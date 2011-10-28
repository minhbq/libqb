/*
 * Copyright (C) 2011 Red Hat, Inc.
 *
 * All rights reserved.
 *
 * Author: Angus Salkeld <asalkeld@redhat.com>
 *
 * libqb is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * libqb is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libqb.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "os_base.h"
#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif /* HAVE_SYSLOG_H */
#include "log_int.h"

static void
_syslog_logger(int32_t target,
	       struct qb_log_callsite *cs, time_t timestamp, const char *msg)
{
#ifdef HAVE_SYSLOG_H
	char output_buffer[QB_LOG_MAX_LEN];
#endif /* HAVE_SYSLOG_H */
	struct qb_log_target *t = qb_log_target_get(target);
	int32_t final_priority = cs->priority + t->priority_bump;

	if (final_priority > LOG_DEBUG) {
		return;
	}
#ifdef HAVE_SYSLOG_H
	qb_log_target_format(target, cs, timestamp, msg, output_buffer);

	if (final_priority < LOG_EMERG) {
		final_priority = LOG_EMERG;
	}
	syslog(final_priority, "%s", output_buffer);
#endif /* HAVE_SYSLOG_H */
}

static void
_syslog_close(int32_t target)
{
#ifdef HAVE_SYSLOG_H
	closelog();
#endif /* HAVE_SYSLOG_H */
}

static void
_syslog_reload(int32_t target)
{
#ifdef HAVE_SYSLOG_H
	struct qb_log_target *t = qb_log_target_get(target);
	closelog();
	openlog(t->name, LOG_PID, t->facility);
#endif /* HAVE_SYSLOG_H */
}

int32_t
qb_log_syslog_open(struct qb_log_target *t)
{
	t->logger = _syslog_logger;
	t->reload = _syslog_reload;
	t->close = _syslog_close;

#ifdef HAVE_SYSLOG_H
	openlog(t->name, LOG_PID, t->facility);
#endif /* HAVE_SYSLOG_H */
	return 0;
}
