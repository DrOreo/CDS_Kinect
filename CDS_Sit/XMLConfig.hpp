/*=========================================================================

Program:
Module:    XMLConfig.hpp

OREO 2016/12/03
=========================================================================*/
#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>
#include "ExceptionDef.h"

using namespace std;

class XMLConfig
{
public:
	XMLConfig();
	~XMLConfig();
public:
	bool ReadXML(string _path);
	bool WriteXML(string _path);

	template<typename valueT>bool PutValue(string, valueT);
	template<typename valueT>valueT GetValue(string);
	template<typename valueT>valueT GetValue(string, valueT);

private:
	bool m_bReadState;
	boost::property_tree::ptree conf;
};


inline XMLConfig::XMLConfig()
{
	this->m_bReadState = false;
}

inline XMLConfig::~XMLConfig()
{

}

inline bool XMLConfig::ReadXML(string _path)
{
	this->m_bReadState = ExceptionEvent([&]()
	{
		boost::property_tree::xml_parser::read_xml(_path, conf, boost::property_tree::xml_parser::trim_whitespace);
	} );

	return this->m_bReadState;
}

inline bool XMLConfig::WriteXML(string _path)
{
	return ExceptionEvent( [&]() 
	{
		//設定存入格式
		boost::property_tree::xml_parser::xml_writer_settings<string> settings('\t', 1, "UTF-8");
		//存入xml文件
		boost::property_tree::write_xml(_path, this->conf, std::locale(), settings);
	} );
}

template<typename valueT>
inline bool XMLConfig::PutValue(string _path, valueT _value)
{
	return ExceptionEvent([&]()
	{
		conf.put<valueT>(_path, _value);
	});
}

template<typename valueT>
inline valueT XMLConfig::GetValue(string _path)
{
	valueT _out;

	ExceptionEvent([&]()
	{
		_out = conf.get<valueT>(_path, boost::lexical_cast<valueT>(0));
	});
	return _out;
}

template<typename valueT>
inline valueT XMLConfig::GetValue(string _path, valueT _default)
{
	valueT _out;
	ExceptionEvent([&]()
	{
		_out = conf.get<valueT>(_path, _default);
	});
	return _out;
}