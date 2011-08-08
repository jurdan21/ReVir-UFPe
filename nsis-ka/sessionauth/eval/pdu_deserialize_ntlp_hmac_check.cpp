#include "pdu_deserialize_ntlp_hmac_check.h"
#include "ntlp_pdu.h"
#include <iostream>

namespace nslp_auth {
  using namespace std;

pdu_deserialize_ntlp_hmac_check::pdu_deserialize_ntlp_hmac_check(bool hmac_correct)
 : hmac_correct(hmac_correct) ,bytes_written(0), msg(new protlib::NetMsg(1000)) {
 	register_NTLP_ies();
 	ntlp::data* data_pdu = create_NTLP_PDU(true);
	uint32 expected_size = data_pdu->get_serialized_size(protlib::IE::protocol_v1);
	data_pdu->serialize(*msg,protlib::IE::protocol_v1,bytes_written);
	if( bytes_written != expected_size )
		cout<<"Error: unexpected pdu size"<<endl;
	if(!hmac_correct){
		uint32 auth_pos = *(msg->get_TLP_list()->get_list(ntlp::ntlp_pdu::tlplist_nslp_object_type, auth_session_object::NSLP_TYPE)->begin());
		uint32 hmac_pos = data_pdu->get_serialized_size(IE::protocol_v1) - auth_session_object::HMAC_size;
  		// set hmac wrong
  		msg->get_buffer()[hmac_pos+8]=0;
	}
	delete data_pdu;
}

pdu_deserialize_ntlp_hmac_check::~pdu_deserialize_ntlp_hmac_check() {
	delete msg;
}

void pdu_deserialize_ntlp_hmac_check::performTask() {
	msg->set_pos(0);
	uint32 bytes_read = 0;
	IEErrorList err;
	IE* ie = ntlp::NTLP_IEManager::instance()->deserialize(*msg, ntlp::NTLP_IE::cat_known_pdu, protlib::IE::protocol_v1, err, bytes_read, false);
	if( bytes_read != bytes_written )
		cout<<"Error: unxpected deserialized size"<<endl;
	if( ( !err.is_empty() && hmac_correct ) || ( err.is_empty() && !hmac_correct )  )
		cout<<"Error: hmac check is wrong or deserialize error occured"<<endl;
	delete ie;
}

}
