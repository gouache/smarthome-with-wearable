#include <glib.h>
#include <net_connection.h>
#include <curl/curl.h>
#include <bluetooth.h>

#include "smarthome.h"

/* HM-10 Service & Characteristic */
#define SERVICE "0000ffe0-0000-1000-8000-00805f9b3"
#define CHARACTERISTIC "0000ffe1-0000-1000-8000-00805f9b34fb"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *naviframe;
	Evas_Object *rs;
	bt_gatt_client_h cli;
} appdata_s;

char *main_menu_names[] = {
	"Car Open",
	"Car Close",
	"Door",
	"Light on",
	"Light off",
	"TV",
	"TV CH",
	"Aircon on",
	"Aircon off",
	"Radio",
	"CCTV",
	NULL
};

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}

static Eina_Bool
_naviframe_pop_cb(void *data, Elm_Object_Item *it)
{
	ui_app_exit();
	return EINA_FALSE;
}

int
__bt_gatt_client_set_value(char *type, char *value, bt_gatt_h h)
{
	int ret;
	int s_val;
	unsigned int u_val;
	char *buf;
	int len;

	if (strcasecmp(type, "int8") == 0) {
		s_val = atoi(value);
		buf = (char *)&s_val;
		len = 1;
	} else if (strcasecmp(type, "int16") == 0) {
		s_val = atoi(value);
		buf = (char *)&s_val;
		len = 2;
	} else if (strcasecmp(type, "int32") == 0) {
		s_val = atoi(value);
		buf = (char *)&s_val;
		len = 4;
	} else if (strcasecmp(type, "uint8") == 0) {
		u_val = strtoul(value, NULL, 10);
		buf = (char *)&u_val;
		len = 1;
	} else if (strcasecmp(type, "uint16") == 0) {
		u_val = strtoul(value, NULL, 10);
		buf = (char *)&u_val;
		len = 2;
	} else if (strcasecmp(type, "uint32") == 0) {
		u_val = strtoul(value, NULL, 10);
		buf = (char *)&u_val;
		len = 4;
	} else if (strcasecmp(type, "str") == 0) {
		buf = value;
		len = strlen(buf);
	} else
		return BT_ERROR_INVALID_PARAMETER;

	ret = bt_gatt_set_value(h, buf, len);
	if (ret != BT_ERROR_NONE)
		dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_set_value failed : %d", ret);

	return ret;
}

void
__bt_gatt_client_open_next_write_complete_cb(int result, bt_gatt_h gatt_handle, void *data)
{
	char *uuid = NULL;
	bt_gatt_get_uuid(gatt_handle, &uuid);

	dlog_print(DLOG_INFO, LOG_TAG, "Write %s for uuid : (%s)",
		result == BT_ERROR_NONE ? "Success" : "Fail", uuid);

	g_free(uuid);
}

void
__bt_gatt_client_open_write_complete_cb(int result, bt_gatt_h gatt_handle, void *data)
{
	char *uuid = NULL;
	bt_gatt_get_uuid(gatt_handle, &uuid);

	dlog_print(DLOG_INFO, LOG_TAG, "Write %s for uuid : (%s)",
		result == BT_ERROR_NONE ? "Success" : "Fail", uuid);

	g_free(uuid);

	sleep(1);
	char wvalue[] = {"AT+PIOB0"};
	int ret = __bt_gatt_client_set_value("str", wvalue, gatt_handle);
	if (ret != BT_ERROR_NONE) {
		dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_set_value failed : %d", ret);
		return;
	}
	ret = bt_gatt_client_write_value(gatt_handle, __bt_gatt_client_open_next_write_complete_cb, NULL);

	if (ret == BT_ERROR_NONE)
		dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_client_write_value : Success");
	else
		dlog_print(DLOG_INFO, LOG_TAG, "failed bt_gatt_client_write_value : %d", ret);
}

void
__bt_gatt_client_open_read_complete_cb(int result, bt_gatt_h gatt_handle, void *data)
{
	char *uuid = NULL;
	bt_gatt_get_uuid(gatt_handle, &uuid);

	dlog_print(DLOG_INFO, LOG_TAG, "Read %s for uuid : (%s)",
		result == BT_ERROR_NONE ? "Success" : "Fail", uuid);

	g_free(uuid);

	char *rvalue;

	int len;
	int ret = bt_gatt_get_value(gatt_handle, &rvalue, &len);
	if (ret != BT_ERROR_NONE) {
		dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_get_value failed : %d", ret);
	} else {
		dlog_print(DLOG_INFO, LOG_TAG, "after value : %d: %s", len, rvalue);

		char wvalue[] = {"AT+PIOB1"};
		ret = __bt_gatt_client_set_value("str", wvalue, gatt_handle);
		if (ret != BT_ERROR_NONE) {
			dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_set_value failed : %d", ret);
			return;
		}
		ret = bt_gatt_client_write_value(gatt_handle, __bt_gatt_client_open_write_complete_cb, NULL);

		if (ret == BT_ERROR_NONE)
			dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_client_write_value : Success");
		else
			dlog_print(DLOG_INFO, LOG_TAG, "failed bt_gatt_client_write_value : %d", ret);
	}
}

void
__bt_gatt_client_close_next_write_complete_cb(int result, bt_gatt_h gatt_handle, void *data)
{
	char *uuid = NULL;
	bt_gatt_get_uuid(gatt_handle, &uuid);

	dlog_print(DLOG_INFO, LOG_TAG, "Write %s for uuid : (%s)",
		result == BT_ERROR_NONE ? "Success" : "Fail", uuid);

	g_free(uuid);
}

void
__bt_gatt_client_close_write_complete_cb(int result, bt_gatt_h gatt_handle, void *data)
{
	char *uuid = NULL;
	bt_gatt_get_uuid(gatt_handle, &uuid);

	dlog_print(DLOG_INFO, LOG_TAG, "Write %s for uuid : (%s)",
		result == BT_ERROR_NONE ? "Success" : "Fail", uuid);

	g_free(uuid);

	sleep(1);
	char wvalue[] = {"AT+PIO70"};
	int ret = __bt_gatt_client_set_value("str", wvalue, gatt_handle);
	if (ret != BT_ERROR_NONE) {
		dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_set_value failed : %d", ret);
		return;
	}
	ret = bt_gatt_client_write_value(gatt_handle, __bt_gatt_client_close_next_write_complete_cb, NULL);

	if (ret == BT_ERROR_NONE)
		dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_client_write_value : Success");
	else
		dlog_print(DLOG_INFO, LOG_TAG, "failed bt_gatt_client_write_value : %d", ret);
}

void
__bt_gatt_client_close_read_complete_cb(int result, bt_gatt_h gatt_handle, void *data)
{
	char *uuid = NULL;
	bt_gatt_get_uuid(gatt_handle, &uuid);

	dlog_print(DLOG_INFO, LOG_TAG, "Read %s for uuid : (%s)",
		result == BT_ERROR_NONE ? "Success" : "Fail", uuid);

	g_free(uuid);

	char *rvalue;

	int len;
	int ret = bt_gatt_get_value(gatt_handle, &rvalue, &len);
	if (ret != BT_ERROR_NONE) {
		dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_get_value failed : %d", ret);
	} else {
		dlog_print(DLOG_INFO, LOG_TAG, "after value : %d: %s", len, rvalue);

		char wvalue[] = {"AT+PIO71"};
		ret = __bt_gatt_client_set_value("str", wvalue, gatt_handle);
		if (ret != BT_ERROR_NONE) {
			dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_set_value failed : %d", ret);
			return;
		}
		ret = bt_gatt_client_write_value(gatt_handle, __bt_gatt_client_close_write_complete_cb, NULL);

		if (ret == BT_ERROR_NONE)
			dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_client_write_value : Success");
		else
			dlog_print(DLOG_INFO, LOG_TAG, "failed bt_gatt_client_write_value : %d", ret);
	}
}


void
item_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	CURL *curl;
	CURLcode res;
	appdata_s *ad = data;

	bt_gatt_h svc = NULL;
	bt_gatt_h chr = NULL;
	int ret;

	Eext_Object_Item * item = eext_rotary_selector_selected_item_get(obj);
	const char *text = eext_rotary_selector_item_part_text_get(item, "selector,main_text");
	if (!strncmp(text, "Car Open", sizeof("Car Open"))){
		ret = bt_gatt_client_get_service(ad->cli, SERVICE, &svc);
		if (ret != BT_ERROR_NONE) {
			dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_client_get_service failed : %d", ret);
		}

		ret = bt_gatt_service_get_characteristic(svc, CHARACTERISTIC, &chr);
		if (ret != BT_ERROR_NONE) {
			dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_service_get_characteristic failed : %d", ret);
		}

		ret = bt_gatt_client_read_value(chr, __bt_gatt_client_open_read_complete_cb, NULL);
		if (ret != BT_ERROR_NONE) {
		   dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_client_read_value failed : %d", ret);
		   return;
		}

		return;
	} else if (!strncmp(text, "Car Close", sizeof("Car Close"))){
		ret = bt_gatt_client_get_service(ad->cli, SERVICE, &svc);
		if (ret != BT_ERROR_NONE) {
			dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_client_get_service failed : %d", ret);
		}

		ret = bt_gatt_service_get_characteristic(svc, CHARACTERISTIC, &chr);
		if (ret != BT_ERROR_NONE) {
			dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_service_get_characteristic failed : %d", ret);
		}

		ret = bt_gatt_client_read_value(chr, __bt_gatt_client_close_read_complete_cb, NULL);
		if (ret != BT_ERROR_NONE) {
		   dlog_print(DLOG_INFO, LOG_TAG, "bt_gatt_client_read_value failed : %d", ret);
		   return;
		}

		return;
	}

	curl = curl_easy_init();
	if(curl) {
		connection_h connection;
		int conn_err;
		conn_err = connection_create(&connection);
		if (conn_err != CONNECTION_ERROR_NONE) {
		   // Error handling
		   return;
		}

		char *proxy_address;
		conn_err = connection_get_proxy(connection, CONNECTION_ADDRESS_FAMILY_IPV4, &proxy_address);
		dlog_print(DLOG_ERROR, LOG_TAG, "proxy url = %s", proxy_address);
		if (!strncmp(text, main_menu_names[0], sizeof(main_menu_names[0]))){
			curl_easy_setopt(curl, CURLOPT_URL, "http://raspberry-pi/car");
		} else if (!strncmp(text, main_menu_names[1], sizeof(main_menu_names[1]))){
			curl_easy_setopt(curl, CURLOPT_URL, "http://raspberry-pi/door");
		}

		if (conn_err == CONNECTION_ERROR_NONE && proxy_address) {
		   curl_easy_setopt(curl, CURLOPT_PROXY, proxy_address);
		}
		res = curl_easy_perform(curl);
		if(res != CURLE_OK){
			dlog_print(DLOG_ERROR, LOG_TAG, "CurlFail", res);
			elm_exit();
		}
		curl_easy_cleanup(curl);
		connection_unset_proxy_address_changed_cb(connection);
		connection_destroy(connection);
	}
}

static void
create_rotary_selector(appdata_s *ad)
{
	Elm_Object_Item *nf_it = NULL;
	int idx = 0;

	ad->rs = eext_rotary_selector_add(ad->naviframe);
	eext_rotary_object_event_activated_set(ad->rs, EINA_TRUE);

	Eext_Object_Item * item;
	for(idx=0; main_menu_names[idx]; idx++) {
		item = eext_rotary_selector_item_append(ad->rs);
		eext_rotary_selector_item_part_text_set(item, "selector,main_text", main_menu_names[idx]);
	}

	evas_object_smart_callback_add(ad->rs, "item,clicked", item_clicked_cb, ad);
	nf_it = elm_naviframe_item_push(ad->naviframe, NULL, NULL, NULL, ad->rs, "empty");
	elm_naviframe_item_pop_cb_set(nf_it, _naviframe_pop_cb, ad->win);
}

static void
create_base_gui(appdata_s *ad)
{
	/* Window */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Conformant */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	ad->naviframe = elm_naviframe_add(ad->conform);
	eext_object_event_callback_add(ad->naviframe, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
	elm_object_content_set(ad->conform, ad->naviframe);

	create_rotary_selector(ad);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);
	ui_app_remove_event_handler(handlers[APP_EVENT_LOW_MEMORY]);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
