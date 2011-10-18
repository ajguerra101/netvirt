/*
 * Generated by asn1c-0.9.23 (http://lionet.info/asn1c)
 * From ASN.1 module "DNDS"
 * 	found in "dnds.asn1"
 */

#ifndef	_ModifyResponse_H_
#define	_ModifyResponse_H_


#include <asn_application.h>

/* Including external dependencies */
#include "DNDSResult.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ModifyResponse */
typedef DNDSResult_t	 ModifyResponse_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ModifyResponse;
asn_struct_free_f ModifyResponse_free;
asn_struct_print_f ModifyResponse_print;
asn_constr_check_f ModifyResponse_constraint;
ber_type_decoder_f ModifyResponse_decode_ber;
der_type_encoder_f ModifyResponse_encode_der;
xer_type_decoder_f ModifyResponse_decode_xer;
xer_type_encoder_f ModifyResponse_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _ModifyResponse_H_ */
#include <asn_internal.h>
