#include "pch.h"
#include "Time.h"

std::chrono::microseconds Time::lastTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
float Time::deltaTime = 0;
std::vector<TimeRecord> Time::records = std::vector<TimeRecord>(8);

void Time::Update() {
	std::chrono::microseconds newTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
	Time::deltaTime = (newTime - Time::lastTime).count() / 1000000.0;
	Time::lastTime = newTime;
}

void Time::PrintDeltaTime() {
	std::cout << Time::deltaTime * 1000 << "ms\n";
}

TimeRecord Time::MeasureStart(const std::string& name) {
	TimeRecord record;
	record.name = name;
	record.start = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
	record.end = std::chrono::microseconds(0);
	Time::records.push_back(record);
	return record;
}

TimeRecord Time::MeasureEnd(const std::string& name) {
	TimeRecord record;
	for (int32_t i = 0; i < Time::records.size(); i++) {
		if (Time::records[i].name == name) {
			record = Time::records[i];
			Time::records.erase(Time::records.begin() + i, Time::records.begin() + i + 1);
			break;
		}
	}
	record.end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
	std::cout << record.name << ": " << (record.end - record.start).count() / 1000.0 << "ms\n";
	return record;
}

std::chrono::microseconds Time::GetTime() {
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
}