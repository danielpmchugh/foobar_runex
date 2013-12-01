

#include "stdafx.h"
#include "RunExWnd.h"
#include "ToolbarBtn.h"

DECLARE_COMPONENT_VERSION(
  "foo_runex", "0.0.9",
"Component creates toolbar button that runs an external command.\n\
////////////////////////////////////////////\n\
// RunEX (foo_runex.dll)\n\
// \n\
// Author: Daniel McHugh (by request from \"derty2\") \n\
////////////////////////////////////////////\n"
);

#include "RunExWndInter.h"

class initquit_runex : public initquit {

	virtual void on_init() 
	{		
		CRunExWndInter::ShowWindow();
	}

	virtual void on_quit() 
	{
		CRunExWndInter::HideWindow();
	}
};

static initquit_factory_t< initquit_runex > foo_initquit;










