#ifndef WIFI_TASK_H_
#define WIFI_TASK_H_


/*WiFi init function*/
void wifi_init(void);
void wifi_deinit(void);
void wifi_ap_init(void);
void wifi_sta_init(void);
void wifi_sta_ap_init(bool hidden);

/*Main WiFi Task*/
void wifi_task(void *pvParameter);

#endif // WIFI_TASK_H_