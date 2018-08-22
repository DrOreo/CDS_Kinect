#include "boost.log.init.hpp"

void g_InitLog()
{
	Blogging::formatter formatter =
		Bexpr::stream
		<< "[" << Bexpr::format_date_time(log_timestamp, "%H:%M:%S")
		<< "]" << Bexpr::if_(Bexpr::has_attr(log_uptime))
		[
			Bexpr::stream << " [" << format_date_time(log_uptime, "%O:%M:%S") << "]"
		]

	<< Bexpr::if_(Bexpr::has_attr(log_scope))
		[
			Bexpr::stream << "[" << Bexpr::format_named_scope(log_scope, Bkeywords::format = "%n") << "]"
		]
	<< "<" << log_severity << ">" << Bexpr::message;

	Blogging::add_common_attributes();

	auto console_sink = Blogging::add_console_log();
	auto file_sink = Blogging::add_file_log
	(
		Bkeywords::file_name = "%Y-%m-%d_%N.log",      //文件名
		Bkeywords::rotation_size = 10 * 1024 * 1024,       //單個文件限制大小
		Bkeywords::time_based_rotation = Bsinks::file::rotation_at_time_point(0, 0, 0)    //每天重建
	);

	file_sink->locked_backend()->set_file_collector(Bsinks::file::make_collector(
		Bkeywords::target = "logs",        //文件夾名
		Bkeywords::max_size = 50 * 1024 * 1024,    //文件夾所佔最大空間
		Bkeywords::min_free_space = 100 * 1024 * 1024  //磁碟最小預留空間
	));

	file_sink->set_filter(log_severity >= Log_Debug);   //日誌級別過濾

	file_sink->locked_backend()->scan_for_files();

	console_sink->set_formatter(formatter);
	file_sink->set_formatter(formatter);
	file_sink->locked_backend()->auto_flush(true);

	Blogging::core::get()->add_global_attribute("Scope", Battrs::named_scope());
	Blogging::core::get()->add_sink(console_sink);
	Blogging::core::get()->add_sink(file_sink);
}
