

#include "stdafx.h"
#include "RunExWnd.h"

DECLARE_COMPONENT_VERSION(
  "foo_runex", "0.0.5",
"Component creates toolbar button that runs an external command.\n\
////////////////////////////////////////////\n\
// RunEX (foo_runex.dll)\n\
// \n\
// Author: Daniel McHugh (by request from \"derty2\") \n\
////////////////////////////////////////////\n"
);

  
class initquit_runex : public initquit {
	virtual void on_init() {		
		CRunExWnd::ShowWindow();		
	}

	virtual void on_quit() {
		// Do nothing.
		// The window placement variable will automatically store the 
		// position and size of our window, if it is currently visible.
	}
};

static initquit_factory_t< initquit_runex > foo_initquit;










