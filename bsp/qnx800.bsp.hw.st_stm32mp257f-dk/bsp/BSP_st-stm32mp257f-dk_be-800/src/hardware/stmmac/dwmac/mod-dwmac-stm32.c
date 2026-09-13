/*
 * io-sock simple sample module
 */

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/proc.h>
#include <sys/sysctl.h>
#include <qnx/qnx_modload.h>

int mod_ver = IOSOCK_VERSION_CUR;

SYSCTL_INT(_qnx_module, OID_AUTO, sample, CTLFLAG_RD, &mod_ver, 0,
            "Version");

static void
run_module(void)
{
  printf("sample module ran\n");
}

static int
module_event_handler(module_t mod, int what, void *arg __unused)
{

  switch (what) {
  case MOD_LOAD:
    run_module();
    break;
  default:
    printf("Unsupported module event:%d\n", what);
    return (EOPNOTSUPP);
}

  return (0);
}

static moduledata_t modsam_moduledata = {
  "module_sample",
  module_event_handler,
  NULL
};

MODULE_VERSION(modsam, 1);
DECLARE_MODULE(modsam, modsam_moduledata, SI_SUB_PSEUDO, SI_ORDER_ANY);

struct _iosock_module_version iosock_module_version =
    IOSOCK_MODULE_VER_SYM_INIT;

static void
modsam_uninit(void *arg)
{
}
/*
 * If code is added to sam_uninit, then SI_SUB_DUMMY needs to be
 * changed to SI_SUB_DRIVERS.  With SI_SUB_DUMMY, sam_uninit does
 * not get called.
 */
SYSUNINIT(modsam_uninit, SI_SUB_DUMMY, SI_ORDER_ANY, modsam_uninit, NULL);
