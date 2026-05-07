/*
 * app.c
 *
 *  Created on: 7 de abr. de 2026
 *      Author: rafael.feijo
 */

#include "app.h"

/** Application Global Var **/
app_t Appl = {0};

/** Calbacks **/
void interface_cb_event(interface_t *interface, interface_events_cb_e e, uint32_t *data)
{
	switch (e)
	{
		case INTERFACE_EVT_ADD_CARDS:

			break;
		case INTERFACE_EVT_LIST_CARDS:

			break;
		case INTERFACE_EVT_DEL_CARDS:

			break;
		case INTERFACE_EVT_DEL_ALL_CARDS:

			break;
		case INTERFACE_EVT_CANCEL:

			break;
		case INTERFACE_EVT_GET_CARDS:

			break;
	}
}
