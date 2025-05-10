#include "pch.h"
#include "KC_common.h"
#include "ClaPathMgr.h"

typedef int (*FN_KC_INIT)(const wchar_t* p_wszServer, int p_nPort);

fn_kc_get_uid kc_get_uid;
fn_kc_get_err kc_get_err;
fn_kc_get_message kc_get_message;
fn_kc_auth kc_auth;
fn_kc_get_session kc_get_session;
fn_kc_otp kc_otp;
fn_kc_get_status kc_get_status;
fn_kc_down_vmlist kc_down_vmlist;
fn_kc_down_vmimage kc_down_vmimage;
fn_kc_down_vmimage_size kc_down_vmimage_size;
fn_kc_up_vmimage_status kc_up_vmimage_status;
fn_kc_down_portlist kc_down_portlist;
fn_kc_down_httpslist kc_down_httpslist;
fn_kc_up_port kc_up_port;
fn_kc_up_violation kc_up_violation;
fn_kc_up_agent_script kc_up_agent_script;
fn_kc_get_agent_version kc_get_agent_version;
fn_kc_up_ip kc_up_ip;
fn_kc_get_idle kc_get_idle;
fn_kc_down_setting kc_down_setting;
fn_kc_down_list kc_down_list;
fn_kc_down_config kc_down_config;
fn_kc_down_check_update kc_down_check_update;
fn_kc_down_update_file kc_down_update_file;
fn_kc_report_violation kc_report_violation;
fn_kc_report_action kc_report_action;
fn_kc_set_password kc_set_password;
fn_kc_get_password kc_get_password;
fn_kc_report_agent_script kc_report_agent_script;
fn_kc_upload_update kc_upload_update;
fn_kc_get_global_ip kc_get_global_ip;
fn_kc_release kc_release;
fn_kc_uninit kc_uninit;
HMODULE hModConn;

int kc_init(const wchar_t* p_wszServer, int p_nPort) {
	CString dllPath = ClaPathMgr::GetDP() + L"\\KenanConn.dll";
	hModConn = LoadLibrary(dllPath);
	if (hModConn == NULL) {
		return -1;
	}

	FN_KC_INIT fn_init = (FN_KC_INIT)GetProcAddress(hModConn, "KC_INIT");
	if (fn_init == NULL) {
		return -2;
	}

	kc_get_err			= (fn_kc_get_err		)GetProcAddress(hModConn, "KC_GET_ERR");
	kc_get_uid			= (fn_kc_get_uid		)GetProcAddress(hModConn, "KC_GET_UID");
	kc_get_message		= (fn_kc_get_message	)GetProcAddress(hModConn, "KC_GET_MESSAGE");
	kc_auth				= (fn_kc_auth			)GetProcAddress(hModConn, "KC_AUTH");
	kc_get_session		= (fn_kc_get_session	)GetProcAddress(hModConn, "KC_GET_SESSION");
	kc_otp				= (fn_kc_otp			)GetProcAddress(hModConn, "KC_OTP");
	kc_get_status		= (fn_kc_get_status		)GetProcAddress(hModConn, "KC_GET_STATUS");
	kc_down_vmlist		= (fn_kc_down_vmlist	)GetProcAddress(hModConn, "KC_DOWN_VMLIST");
	kc_down_vmimage		= (fn_kc_down_vmimage	)GetProcAddress(hModConn, "KC_DOWN_VMIMAGE");
	kc_down_vmimage_size= (fn_kc_down_vmimage_size)GetProcAddress(hModConn, "KC_DOWN_VMIMAGE_SIZE");
	kc_up_vmimage_status= (fn_kc_up_vmimage_status)GetProcAddress(hModConn, "KC_UP_VMIMAGE_STATUS");
	kc_down_portlist	= (fn_kc_down_portlist	)GetProcAddress(hModConn, "KC_DOWN_PORTLIST");
	kc_down_httpslist	= (fn_kc_down_httpslist	)GetProcAddress(hModConn, "KC_DOWN_HTTPSLIST");
	kc_up_port			= (fn_kc_up_port		)GetProcAddress(hModConn, "KC_UP_PORT");
	kc_up_violation		= (fn_kc_up_violation	)GetProcAddress(hModConn, "KC_UP_VIOLATION");
	kc_up_agent_script  = (fn_kc_up_agent_script)GetProcAddress(hModConn, "KC_UP_AGENT_SCRIPT");
	kc_up_ip			= (fn_kc_up_ip			)GetProcAddress(hModConn, "KC_UP_IP");
	kc_upload_update	= (fn_kc_upload_update	)GetProcAddress(hModConn, "KC_UPLOAD_UPDATE");
	kc_get_agent_version= (fn_kc_get_agent_version)GetProcAddress(hModConn, "KC_GET_AGENT_VERSION");

	kc_get_idle			= (fn_kc_get_idle		)GetProcAddress(hModConn, "KC_GET_IDLE");
	kc_down_setting		= (fn_kc_down_setting	)GetProcAddress(hModConn, "KC_DOWN_SETTING");

	kc_down_list		= (fn_kc_down_list		)GetProcAddress(hModConn, "KC_DOWN_LIST");
	kc_down_config		= (fn_kc_down_config	)GetProcAddress(hModConn, "KC_DOWN_CONFIG");
	kc_down_check_update = (fn_kc_down_check_update )GetProcAddress(hModConn, "KC_DOWN_CHECK_UPDATE");
	kc_down_update_file = (fn_kc_down_update_file )GetProcAddress(hModConn, "KC_DOWN_UPDATE_FILE");
	kc_report_action    = (fn_kc_report_action	)GetProcAddress(hModConn, "KC_REPORT_ACTION");
	kc_get_password		= (fn_kc_get_password	)GetProcAddress(hModConn, "KC_GET_PASSWORD");
	kc_set_password		= (fn_kc_set_password	)GetProcAddress(hModConn, "KC_SET_PASSWORD");
	kc_report_violation = (fn_kc_report_violation)GetProcAddress(hModConn, "KC_REPORT_VIOLATION");
	kc_report_agent_script = (fn_kc_report_agent_script)GetProcAddress(hModConn, "KC_REPORT_AGENT_SCRIPT");

	kc_get_global_ip	= (fn_kc_get_global_ip	)GetProcAddress(hModConn, "KC_GET_GLOBAL_IP");
	kc_release			= (fn_kc_release		)GetProcAddress(hModConn, "KC_RELEASE");

	kc_uninit			= (fn_kc_uninit			)GetProcAddress(hModConn, "KC_UNINIT");

	return fn_init(p_wszServer, p_nPort);
}