#ifndef __ATOM_MAYAEXP_CONFIG_H
#define __ATOM_MAYAEXP_CONFIG_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <string>

struct ExporterConfig
{
	std::string shareDirectory;
	std::string rootDirectory;

	ExporterConfig (void);
	~ExporterConfig (void);
	std::string getFileName (void) const;
	void load (void);
	void save (void);
	static ExporterConfig &getInstance(void);
};

#endif // __ATOM_MAYAEXP_CONFIG_H


