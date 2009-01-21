#include "safe_getline.h"

std::istream &safe_getline(std::istream &in_stream, std::string &result){
	result.clear();
	char c;
	while(in_stream.get(c)){
		if(c==10 || c==13){
			int nc=in_stream.peek();
			if( nc!=c && (nc==10 || nc==13)){
				in_stream.get(c);
			}
			break;
		}else{
			result+=c;
		}
	}
	return in_stream;
}
