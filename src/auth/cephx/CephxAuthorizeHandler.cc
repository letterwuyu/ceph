#include "CephxProtocol.h"
#include "CephxAuthorizeHandler.h"
#include "common/dout.h"

#define dout_subsys ceph_subsys_auth



bool CephxAuthorizeHandler::verify_authorizer(
  CephContext *cct, KeyStore *keys,
  bufferlist& authorizer_data, bufferlist& authorizer_reply,
  EntityName& entity_name, uint64_t& global_id, AuthCapsInfo& caps_info,
  CryptoKey& session_key, uint64_t *auid,
  std::unique_ptr<AuthAuthorizerChallenge> *challenge)
{
  auto iter = authorizer_data.cbegin();

  if (!authorizer_data.length()) {
    ldout(cct, 1) << "verify authorizer, authorizer_data.length()=0" << dendl;
    return false;
  }

  CephXServiceTicketInfo auth_ticket_info;

  bool isvalid = cephx_verify_authorizer(cct, keys, iter, auth_ticket_info, challenge,
					 authorizer_reply);

  if (isvalid) {
    caps_info = auth_ticket_info.ticket.caps;
    entity_name = auth_ticket_info.ticket.name;
    global_id = auth_ticket_info.ticket.global_id;
    session_key = auth_ticket_info.session_key;
    if (auid) *auid = auth_ticket_info.ticket.auid;
  }

  return isvalid;
}

// Return type of crypto used for this session's data;  for cephx, symmetric authentication

int CephxAuthorizeHandler::authorizer_session_crypto() 
{
  return SESSION_SYMMETRIC_AUTHENTICATE;
}
