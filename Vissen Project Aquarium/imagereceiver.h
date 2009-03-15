#ifndef IMAGERECEIVER_H_
#define IMAGERECEIVER_H_

#include <string>
#include <vector>

#include "AquariumController.h"

class SimpleIStream{
  public:
  virtual size_t Read(char *data, size_t max_size)=0;
  virtual bool CanRead()=0;/// returns false when cant read anymore (eof)
  virtual ~SimpleIStream(){};
};

class SimpleOStream{
  public:
  virtual size_t Write(const char *data, size_t max_size)=0;
  virtual bool WriteString(const std::string &line)=0;
  virtual bool CanWrite()=0;/// returns false when cant write anymore (write error)
  virtual ~SimpleOStream(){};
};

class SimpleIOStream: public SimpleIStream, SimpleOStream
{
};

class SimpleSocketStream: public SimpleIOStream
{
  int m_sock;/// -1: no socket assigned, any other value might be a socket
  bool m_is_ok;/// false if there was errors other than EWOULDBLOCK etc

  std::string m_error_string;

  void do_ClientOpen(const std::string &hostname, int port);
  public:
  virtual size_t Read(char *data, size_t max_size);
  virtual bool CanRead();

  virtual size_t Write(const char *data, size_t max_size);
  bool WriteString(const std::string &line);
  virtual bool CanWrite();/// returns false when cant write anymore (write error)

  ///SimpleSocketStream();//{SimpleSocketStream(-1)};
  SimpleSocketStream(const std::string &hostname, int port);
  explicit SimpleSocketStream(int sock_=-1);
  ~SimpleSocketStream();

  void ClientOpen(const std::string &hostname, int port){do_ClientOpen(hostname,port);};
  void SetSocket(int sock_);
  int GetSocket(){return m_sock;}
  void Close();
  std::string ErrorString(){return m_error_string;}
};


class ImageReceiver
{
	int m_server_socket;
	SimpleSocketStream m_socket_stream;
	std::vector<unsigned char> buffer;/// contains image
	size_t buffered_bytes;
	size_t image_size;

	void AcceptClient();
	bool ReceiveSegment();

	void ReceivedImage();

	unsigned int texture_id;

	public:
	/// port: the port to listen on.
		ImageReceiver(int port=7779);
		virtual ~ImageReceiver() {};
		void Update();

		unsigned int TextureID();
};

class PositionReceiver
{
	int m_server_socket;
	SimpleSocketStream m_socket_stream;
	std::vector<unsigned char> buffer;/// contains image
	int buffered_bytes;
	int image_size;
	int type;

	void AcceptClient();
	void ReceiveSegment(AquariumController& aquariumController);

	public:
	/// port: the port to listen on.
		PositionReceiver(int type, int port=7779);
		virtual ~PositionReceiver() {};
		void Update(AquariumController& aquariumController);

		unsigned int TextureID();
};

#endif // IMAGERECEIVER_H
