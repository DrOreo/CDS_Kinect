#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>
#include <iostream>
#include "ExceptionDef.h"

using namespace std;

class INIConfig
{
public:
	INIConfig();
	~INIConfig();
public:
	bool ReadINI(string _path);
	bool WriteINI(string _path);

	template<typename valueT>bool PutValue(string, valueT);
	template<typename valueT>valueT GetValue(string);
	template<typename valueT>valueT GetValue(string, valueT);
private:
	bool m_bReadState;
	boost::property_tree::ptree conf;
};


inline INIConfig::INIConfig()
{
	this->m_bReadState = false;
}

inline INIConfig::~INIConfig()
{

}

inline bool INIConfig::ReadINI(string _path)
{
	this->m_bReadState = ExceptionEvent([&]()
	{
		boost::property_tree::ini_parser::read_ini(_path, conf);
	});

	return this->m_bReadState;
}

inline bool INIConfig::WriteINI(string _path)
{
	return ExceptionEvent([&]()
	{
		//存入ini文件
		boost::property_tree::write_ini(_path, this->conf);
	});
}

template<typename valueT>
inline bool INIConfig::PutValue(string _path, valueT _value)
{
	return ExceptionEvent([&]()
	{
		conf.put<valueT>(_path, _value);
	});
}

template<typename valueT>
inline valueT INIConfig::GetValue(string _path)
{
	valueT _out;

	ExceptionEvent([&]()
	{
		_out = conf.get<valueT>(_path, boost::lexical_cast<valueT>(0));
	});
	return _out;
}

template<typename valueT>
inline valueT INIConfig::GetValue(string _path, valueT _default)
{
	valueT _out;
	ExceptionEvent([&]()
	{
		_out = conf.get<valueT>(_path, _default);
	});
	return _out;
}