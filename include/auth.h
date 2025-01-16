// gtklock
// Copyright (c) 2022 Jovan Lanik

// PAM Authentication

#pragma once

#include <glib.h>
#include <security/pam_appl.h>

#include "message_q.h"

enum pwcheck {
	PW_WAIT,
	PW_FAILURE,
	PW_SUCCESS,
	PW_ERROR,
	PW_MESSAGE,
};

enum pipedir {
	PIPE_PARENT,
	PIPE_CHILD,
	PIPE_LAST,
};

typedef int pipe_t[PIPE_LAST];

struct conv_data {
	gboolean error;
	const char *pw;
	gpointer ctx;
};

//char *auth_get_error(void);
//char *auth_get_message(void);

enum pwcheck auth_pw_check(const char *s);
int start_authentication(
	int (*convers)(int, const struct pam_message **, struct pam_response **, void *),
	struct conv_data data
);
