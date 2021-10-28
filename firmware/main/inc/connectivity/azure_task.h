#ifndef AZURE_TASK_H_
#define AZURE_TASK_H_

int azure_init(void);
void azure_deinit();

void azure_task(void *pvParameter);

void azure_tx_task(void *pvParameter);


/*----------- AZURE ENVIRONMENT CONFIGURATION -----------*/
#define AZURE_DPS_ADDRESS       "global.azure-devices-provisioning.net"

#ifdef CONFIG_AZURE_ENVIRONMENT_CUSTOM
    #define AZURE_DPS_SCOPE_ID      CONFIG_DPS_ID_SCOPE
    #define AZURE_ENVIRONMENT       "CUSTOM"
#endif

#ifdef CONFIG_AZURE_ENVIRONMENT_TEST
    #define AZURE_DPS_SCOPE_ID      "0ne000EDAEC"
    #define AZURE_ENVIRONMENT       "TEST"
#endif

#ifdef CONFIG_AZURE_ENVIRONMENT_PRE_PROD
    #define AZURE_DPS_SCOPE_ID      "0ne00103A34"
    #define AZURE_ENVIRONMENT       "PRE-PRODUCTION"
#endif

#ifdef CONFIG_AZURE_ENVIRONMENT_PROD
    #define AZURE_DPS_SCOPE_ID      "0ne001433BA"
    #define AZURE_ENVIRONMENT       "PRODUCTION"
#endif

#ifndef CONFIG_AZURE_ENABLE_LOW_LEVEL_DEBUG
    #define AZURE_DEBUG_ENABLED    false
#else
    #define AZURE_DEBUG_ENABLED    CONFIG_AZURE_ENABLE_LOW_LEVEL_DEBUG
#endif

#endif // AZURE_TASK_H_