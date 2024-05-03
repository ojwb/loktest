/* loktest.cc - Minimal LibreOfficeKit program
 *
 * Copyright (C) 2014,2015,2016,2018,2019,2021 Olly Betts
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstdlib>
#include <exception>
#include <iostream>

#include <LibreOfficeKit/LibreOfficeKit.hxx>

using namespace std;
using namespace lok;

int
main(int argc, char **argv)
{
    if (argc < 3 || argc > 5) {
	cerr << "Syntax: loktest INPUT_URL OUTPUT_URL [FORMAT [OPTIONS]]\n";
	_Exit(1);
    }

    const char * input_url = argv[1];
    const char * output_url = argv[2];
    const char * format = argv[3];
    const char * options = argc > 4 ? argv[4] : NULL;

    const char * lo_path = getenv("LO_PATH");
    if (!lo_path) {
	cerr << "Set LO_PATH in the environment to the 'program' directory - e.g.:\n"
	"LO_PATH=/opt/libreoffice/program\n"
	"export LO_PATH" << endl;
	_Exit(1);
    }

    Office * llo = NULL;
    try {
	cerr << "Calling lok_cpp_init()\n";
	llo = lok_cpp_init(lo_path);
	if (!llo) {
	    cerr << "Failed to initialise LibreOfficeKit" << endl;
	    _Exit(1);
	}
    } catch (const exception & e) {
	delete llo;
	cerr << "lok_cpp_init() threw exception (" << e.what() << ")" << endl;
	_Exit(1);
    }
    cerr << "lok_cpp_init() succeeded\n";

    try {
	cerr << "Calling documentLoad()\n";
	Document * lodoc = llo->documentLoad(input_url, options);
	if (!lodoc) {
	    const char * errmsg = llo->getError();
	    cerr << "LibreOfficeKit failed to load document (" << errmsg << ")" << endl;
	    _Exit(1);
	}
	cerr << "documentLoad() succeeded\n";

	cerr << "Calling saveAs()\n";
	if (!lodoc->saveAs(output_url, format, options)) {
	    const char * errmsg = llo->getError();
	    cerr << "LibreOfficeKit failed to export (" << errmsg << ")" << endl;
	    delete lodoc;
	    _Exit(1);
	}
	cerr << "saveAs() succeeded\n";

	cerr << "deleting Document\n";
	delete lodoc;
    } catch (const exception & e) {
	cerr << "documentLoad() or saveAs() threw exception (" << e.what() << ")" << endl;
	_Exit(1);
    }
 
    cerr << "deleting Office\n";
    delete llo;

    cerr << "Done\n";
    _Exit(0);
}
