/*=========================================================================

Program:
Module:    ExceptionDef.h

OREO 2016/12/03
=========================================================================*/
#pragma once


//return ExceptionEvent( [&](){to do...} );
template<typename Func>
bool ExceptionEvent(Func&& f)
{
	try
	{
		f();
		return true;
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
	return false;
}

