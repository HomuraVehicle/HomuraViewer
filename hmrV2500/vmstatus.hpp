#ifndef HMR_VMSTATUS_INC
#define HMR_VMSTATUS_INC 100
#
namespace hmr {
	template<typename T>
	class vmstatus {
		T Machine;
		T Viewer;
	public:
		vmstatus(const T& Viewer_Default=T(), const T& Machine_Default=T())
			:Viewer(Viewer_Default), Machine(Machine_Default){
		}
	public:
		void set(const T& Viewer_, const T& Machine_) {
			Viewer=Viewer_;
			Machine=Machine_;
		}
		void setv(const T& Viewer_) {
			Viewer=Viewer_;
		}
		void setm(const T& Machine_) {
			Machine=Machine_;
		}
		const T& getv(void)const { return Viewer; }
		const T& getm(void)const { return Machine; }
		bool operator()(void) { return Viewer!=Machine; }
	};
	typedef vmstatus<bool> vmflag;
}
#
#endif
