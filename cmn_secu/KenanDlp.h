#pragma once

#include "kn_conf.h"

unsigned int dlp_init();
unsigned int dlp_mount(ST_SETTING* p_pstSetting);
unsigned int dlp_launch_new(ST_CONF_APP* p_pstApp);
unsigned int dlp_finish(bool p_bMsg);
bool dlp_is_exist_secu();