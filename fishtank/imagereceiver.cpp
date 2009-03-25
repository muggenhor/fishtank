#include "imagereceiver.h"
#include <GL/glu.h>
#include "AquariumController.h"

#include <algorithm>

#include <sys/types.h>

#include <string.h>
#include <sstream>
#include <iostream>

#ifdef WIN32
#include <winsock2.h>
#include <io.h>
#include <ws2tcpip.h>

class StartupHax{/// need for windows
public:
	WSADATA stuff;
	StartupHax(){
		WORD ver_required=(2<<8)+2;
		int i=WSAStartup(ver_required, &stuff);
		if (i!=0){
			std::cout<<"WSAStartup() failed."<<std::endl;
		}
	}
};
StartupHax StartupHax_init;

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#endif

#include <fcntl.h>
#include <errno.h>


bool SimpleSocketStream::WriteString(const std::string &line)
{
	const size_t s = Write(&line[0],line.size());
	if (s != line.size())
	{
		//wxLogMessage(_T("Socket write failed, size=%d"),s);
		std::ostringstream ss;
		ss<<"write failed: want to write "<<line.size()<<" but only wrote "<<s;
		m_error_string=ss.str();
		return false;
	}

	return true;
}


size_t SimpleSocketStream::Read(char *data, size_t max_size){
	if (m_sock==-1){
		m_is_ok=false;
		return 0;
	}
	int e;
	int actual_size;
	/* emulate MSG_DONTWAIT for moronic windoze. */
#ifdef WIN32
	fd_set rfds1;
	FD_ZERO (&rfds1);
	FD_SET (m_sock, &rfds1);
	timeval timev;
	timev.tv_sec=0;
	timev.tv_usec=1;
	if (select ((int)m_sock, &rfds1, NULL, NULL, &timev)) {
		actual_size=recv(m_sock,data,max_size,0);
	}else{
		actual_size=-1;
		e=EAGAIN;
	}
#else
	actual_size=recv(m_sock,data,max_size,MSG_DONTWAIT);
	e=errno;
#endif
	if (actual_size<0){
		actual_size=0;

		switch (e){
		case EAGAIN:
			//std::cout<<"eagain"<<std::endl;
			break;
		default:
			//std::cout<<"read error"<<std::endl;
			m_is_ok=false;
		}
	}
	return actual_size;
}

bool SimpleSocketStream::CanRead(){
	return (m_sock!=-1)&&m_is_ok;
}

size_t SimpleSocketStream::Write(const char *data, size_t max_size){
	if (m_sock==-1)return 0;
	int actual_size=send(m_sock,const_cast<char *>(data),max_size,0/*MSG_NOSIGNAL*//*MSG_DONTWAIT*/);
	if (actual_size<0){
		actual_size=0;
		int e=errno;
		switch (e){
		case EAGAIN:
			break;
		default:
			m_is_ok=false;
		}
	}
	return actual_size;
}

bool SimpleSocketStream::CanWrite(){
	return (m_sock!=-1)&&m_is_ok;
}

SimpleSocketStream::SimpleSocketStream(const std::string &hostname, int port):
		m_sock(-1),
		m_is_ok(false)
{
	do_ClientOpen(hostname,port);
}
SimpleSocketStream::SimpleSocketStream(int sock_):
		m_sock(sock_),
		m_is_ok(sock_!=-1)
{
}

SimpleSocketStream::~SimpleSocketStream()
{
	if (m_sock!=-1)close(m_sock);
}

void SimpleSocketStream::do_ClientOpen(const std::string &hostname, int port){
	Close();
	hostent *host=gethostbyname(hostname.c_str());
	if (!host){
		m_error_string="cant resolve host "+hostname;
		return;
	}
	sockaddr_in server_address;
	memset(&server_address,0,sizeof(server_address));
	server_address.sin_family = host->h_addrtype;
	memcpy((char *) &server_address.sin_addr,
				 host->h_addr_list[0], sizeof(server_address.sin_addr) );
	server_address.sin_port = htons(port);

	m_sock=socket (PF_INET, SOCK_STREAM, 0);
	if (m_sock<0){
		m_error_string="cant create socket";
		return;
	}
	if (connect(m_sock,(sockaddr *)&server_address,sizeof(server_address))<0){
		m_error_string="cant connect socket";
		return;
	}
	m_is_ok=true;
}

void SimpleSocketStream::SetSocket(int sock_){
	//if (m_sock!=-1)close(m_sock);
	m_sock=sock_;
	m_is_ok=true;
}
void SimpleSocketStream::Close(){
	if (m_sock!=-1){
		close(m_sock);
		m_sock=-1;
	}
	m_is_ok=false;
}


const short BITMAP_MAGIC_NUMBER=19778;
const int RGB_BYTE_SIZE=3;

/// taken from http://www.gamedev.net/reference/articles/article1966.asp
#pragma pack(push,bitmap_data,1)
typedef struct tagRGBQuad {
	char rgbBlue;
	char rgbGreen;
	char rgbRed;
	char rgbReserved;
} RGBQuad;

typedef struct tagBitmapFileHeader {
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
} BitmapFileHeader;

typedef struct tagBitmapInfoHeader {
	unsigned int biSize;
	int biWidth;
	int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
} BitmapInfoHeader;
#pragma pack(pop,bitmap_data)

static const size_t header_size=sizeof(BitmapFileHeader)+sizeof(BitmapInfoHeader);

ImageReceiver::ImageReceiver(int port)
{
	texture_id=0;
	buffered_bytes=0;
	image_size=0;

	sockaddr_in server_address;
	memset(&server_address,0,sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_port=htons(port);
	server_address.sin_addr.s_addr = htonl (INADDR_ANY);
	m_server_socket=socket (PF_INET, SOCK_STREAM, 0);
	if (m_server_socket < 0)
	{
		std::cerr<<"Error creating socket!"<<std::endl;
	}
	if (bind(m_server_socket, (sockaddr *)&server_address, sizeof(server_address)) < 0){
		std::cerr<<"Error binding socket!"<<std::endl;
	}
	std::cerr<<"Listening on port "<<port<<std::endl;
	listen(m_server_socket, 5);


#ifdef WIN32
	u_long NonBlock = 1;
	if (ioctlsocket(m_server_socket, FIONBIO, &NonBlock) == SOCKET_ERROR)
  {
		std::cerr<<"ioctlsocket() failed \n"<<std::endl;
		return;
  }
#else
	fcntl(m_server_socket, F_SETFL, O_NONBLOCK);
#endif

}

void ImageReceiver::AcceptClient(){
	sockaddr_in client_address;
	memset(&client_address,0,sizeof(client_address));
	socklen_t client_address_len=sizeof(client_address);
	int client_socket=accept(m_server_socket,(struct sockaddr *) &client_address, &client_address_len);
	if (client_socket<0){
		//std::cerr<<"accept() failed"<<std::endl;
		return;
	}
	m_socket_stream.SetSocket(client_socket);
	std::string client_address_str=inet_ntoa(client_address.sin_addr);
	std::cerr<<client_address_str<<" connected"<<std::endl;
}

void ImageReceiver::Update(){
	if(!m_socket_stream.CanRead()){
		/*
		pollfd pfd;
		memset(&pfd,0,sizeof(pfd));
		pfd.fd=m_sock;
    pfd.events=POLLIN;
    */
    AcceptClient();
	}else{
		while(ReceiveSegment()){};/// receive segments.
	}
}

bool ImageReceiver::ReceiveSegment(){
	if(buffered_bytes>=header_size && image_size==0){
		BitmapFileHeader *header=(BitmapFileHeader *)(&buffer[0]);
		image_size=header->bfSize;
		if(header->bfType!=BITMAP_MAGIC_NUMBER || image_size<buffered_bytes){
			std::cerr<<"Stream is corrupt(1), closing connection!"<<std::endl;
			m_socket_stream.Close();
			return false;
		}
		buffer.resize(image_size);/// header pointer is not valid anymore after resize
	}

	const size_t wanted_buffer_size = image_size>header_size ? image_size : header_size;
	if(buffer.size()<wanted_buffer_size)buffer.resize(wanted_buffer_size);
	const int bytes_to_receive=wanted_buffer_size-buffered_bytes;
	if(bytes_to_receive<0){
		std::cerr<<"Stream is corrupt(2), closing connection!"<<std::endl;
		m_socket_stream.Close();
		return false;
	}
	if(bytes_to_receive==0){
		if(image_size>0){/// got complete image
			ReceivedImage();
			image_size=0;/// clear buffer.
			buffered_bytes=0;
			return false;/// dont retry load till next frame.
		}else{/// got just header, wtf, should never be here
			std::cerr<<"Something wrong in receive"<<std::endl;
			return false;
		}
	}
	int received_size=m_socket_stream.Read((char*)(&buffer[buffered_bytes]), bytes_to_receive);
	if(received_size<0)return false;
	buffered_bytes+=received_size;
	return received_size!=0;/// if received something, continue looping
}

unsigned int ImageReceiver::TextureID(){
	return texture_id;
}

using namespace std;
void ImageReceiver::ReceivedImage()
{
	//std::cerr<<"Received an image"<<std::endl;
	BitmapInfoHeader *info=(BitmapInfoHeader *)(&buffer[sizeof(BitmapFileHeader)]);
	//std::cerr<<"resolution: "<<(info->biWidth)<<"x"<<(info->biHeight)<<std::endl;
/*
	cout << "biBitCount      =" << bmih.biBitCount << endl;
	cout << "biClrImportant  =" << bmih.biClrImportant << endl;
	cout << "biClrUsed       =" << bmih.biClrUsed << endl;
	cout << "biCompression   =" << bmih.biCompression << endl;
	cout << "biHeight        =" << bmih.biHeight << endl;
	cout << "biPlanes        =" << bmih.biPlanes << endl;
	cout << "biSize          =" << bmih.biSize << endl;
	cout << "biSizeImage     =" << bmih.biSizeImage << endl;
	cout << "biWidth         =" << bmih.biWidth << endl;
	cout << "biXPelsPerMeter =" << bmih.biXPelsPerMeter << endl;
	cout << "biYPelsPerMeter =" << bmih.biYPelsPerMeter << endl;
	*/
	if(info->biBitCount!=24){
		std::cerr<<"Error: image isnt 24 bits per pixel"<<std::endl;
		return;
	}

	if(!texture_id){/// got no texture ID yet.
		glGenTextures(1, &texture_id);
	}
	glBindTexture(GL_TEXTURE_2D, texture_id);

	int width=info->biWidth;
	int height=abs(info->biHeight);/// silly format can have negative height. o_0

	if(buffer.size()<header_size+width*height*3){
			std::cerr<<"Error: image dimensions incorrect"<<std::endl;
		return;
	}
	int last=header_size+width*height*3;
	for(int i=header_size; i<last; i+=3){/// convert
		unsigned char tmp=buffer[i];
		buffer[i]=buffer[i+2];
		buffer[i+2]=tmp;
	}
	glGetError();
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &buffer[header_size]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, &buffer[header_size]);
	int i=glGetError();
	if(i!=GL_NO_ERROR){
		std::cerr<<"Error: glTexImage2D failed"<<std::endl;
	}

	glBindTexture(GL_TEXTURE_2D,0);
}







#pragma pack(push,bitmap_data,1)

typedef struct tagPositionInfoHeader {
	unsigned char x;
	unsigned char y;
} PositionInfoHeader;
#pragma pack(pop,bitmap_data)

const int header_size_position=sizeof(tagPositionInfoHeader);

PositionReceiver::PositionReceiver(int type, int port) :
	buffered_bytes(0),
	image_size(0),
	type(type)
{
	sockaddr_in server_address;
	memset(&server_address,0,sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_port=htons(port);
	server_address.sin_addr.s_addr = htonl (INADDR_ANY);
	m_server_socket=socket (PF_INET, SOCK_STREAM, 0);
	if (m_server_socket < 0)
	{
		std::cerr<<"Error creating socket!"<<std::endl;
	}
	if (bind(m_server_socket, (sockaddr *)&server_address, sizeof(server_address)) < 0){
		std::cerr<<"Error binding socket!"<<std::endl;
	}
	std::cerr<<"Listening on port "<<port<<std::endl;
	listen(m_server_socket, 5);


#ifdef WIN32
	u_long NonBlock = 1;
	if (ioctlsocket(m_server_socket, FIONBIO, &NonBlock) == SOCKET_ERROR)
  {
		std::cerr<<"ioctlsocket() failed \n"<<std::endl;
		return;
  }
#else
	fcntl(m_server_socket, F_SETFL, O_NONBLOCK);
#endif

}

void PositionReceiver::AcceptClient(){
	sockaddr_in client_address;
	memset(&client_address,0,sizeof(client_address));
	socklen_t client_address_len=sizeof(client_address);
	int client_socket=accept(m_server_socket,(struct sockaddr *) &client_address, &client_address_len);
	if (client_socket<0){
		//std::cerr<<"accept() failed"<<std::endl;
		return;
	}
	m_socket_stream.SetSocket(client_socket);
	std::string client_address_str=inet_ntoa(client_address.sin_addr);
	std::cerr<<client_address_str<<" connected"<<std::endl;
}

void PositionReceiver::Update(AquariumController& aquariumController)
{
	if(!m_socket_stream.CanRead()){
		/*
		pollfd pfd;
		memset(&pfd,0,sizeof(pfd));
		pfd.fd=m_sock;
    pfd.events=POLLIN;
    */
    AcceptClient();
	}else{
		ReceiveSegment(aquariumController);/// receive segments.
	}
}

void PositionReceiver::ReceiveSegment(AquariumController& aquariumController)
{
	PositionInfoHeader pos;
	int received_size = m_socket_stream.Read((char*)(&pos), 2);

	if (received_size == 2)
	{
		// Received position is a percentage, we need a fraction.
		const Eigen::Vector2d position(Eigen::Vector2d(pos.x, pos.y)
		                              / 100.);

		if (type == 0)
		{
			aquariumController.GoToScreen(position);
			cerr << "Zwempositie: " << position << "\n";
		}
		else
		{
			aquariumController.facePosition = position;
			cerr << "Gezichtpositie: " << position << "\n";
		}
	}
}
