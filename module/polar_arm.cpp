/*
 * polar_arm.cpp
 *
 *  Created on: 2019/08/28
 *      Author: mutsuro
 */

#include "polar_arm.h"


PolarArm::PolarArm(float offset_x, float offset_y, float offset_z, float offset_slider) :
	x_offset(offset_x), y_offset(offset_y), z_offset(offset_z), slider_offset(offset_slider)
{
	x.target = INIT_X - x_offset;
	y.target = INIT_Y - y_offset;
	z.target = INIT_Z - z_offset;
	mode_slider = Mode::Backward;
	restart(INIT_X, INIT_Y, INIT_Z);
}


// 座標はフィールド基準
void PolarArm::restart(float target_x, float target_y, float target_z)
{
	// 初期位置の更新
	x.start = x.target;
	y.start = y.target;
	z.start = z.target;
	cartesian2polar(x.start, y.start, z.start, &r.start, &theta.start, &phi.start);

	// 目標位置の更新
	// offset処理も忘れずに
	set_target(target_x, target_y, target_z);

	clock.reset();
	clock.start();
}


// 座標はフィールド基準
void PolarArm::set_target(float target_x, float target_y, float target_z)
{
	if (mode_slider == Mode::Forward) target_y -= slider_offset;
	x.target = target_x - x_offset;
	y.target = target_y - y_offset;
	z.target = target_z - z_offset;
	x.dist = x.target - x.start;
	y.dist = y.target - y.start;
	z.dist = z.target - z.start;

	cartesian2polar(x.target, y.target, z.target, &r.target, &theta.target, &phi.target);
	r.dist 		= r.target - r.start;
	theta.dist 	= theta.target - theta.start;
	phi.dist 	= phi.target - phi.start;
}


void PolarArm::set_duration(float duration)
{
	this->duration = duration;
}


void PolarArm::set_mode(int mode)
{
	mode_coord = (Mode::Coordinate)Mode::coordinate(mode);
	mode_acc = (Mode::Accelaration)Mode::accelaration(mode);
	mode_slider = (Mode::Slider)Mode::slider(mode);
}


void PolarArm::calc_next()
{
	float clock_now = clock.read();

	// r, theta, phi のnextを求める
	switch (mode_coord) {
	case Mode::Cartesian:
		calc_pos_cartesian(clock_now);
		cartesian2polar(x.next, y.next, z.next, &r.next, &theta.next, &phi.next);
		break;
	case Mode::Polar:
		calc_pos_polar(clock_now);
		break;
	}
}


float PolarArm::get_r_next()
{
	return r.next;
}


float PolarArm::get_theta_next()
{
	return theta.next;
}


float PolarArm::get_phi_next()
{
	return phi.next;
}


void PolarArm::calc_pos_cartesian(float now_t)
{
	switch (mode_acc) {
	case Mode::LinearAcc:
		x.next = x.start + linear_accel_pos(duration, x.dist, now_t);
		y.next = y.start + linear_accel_pos(duration, y.dist, now_t);
		z.next = z.start + linear_accel_pos(duration, z.dist, now_t);
		break;
	case Mode::NonLinearAcc:
		x.next = x.start + sin_accel_pos(duration, x.dist, now_t);
		y.next = y.start + sin_accel_pos(duration, y.dist, now_t);
		z.next = z.start + sin_accel_pos(duration, z.dist, now_t);
		break;
	}
}


void PolarArm::calc_pos_polar(float now_t)
{
	switch (mode_acc) {
	case Mode::LinearAcc:
		r.next 		= r.start + linear_accel_pos(duration, r.dist, now_t);
		theta.next 	= theta.start + linear_accel_pos(duration, theta.dist, now_t);
		phi.next 	= phi.start + linear_accel_pos(duration, phi.dist, now_t);
		break;
	case Mode::NonLinearAcc:
		r.next 		= r.start + sin_accel_pos(duration, r.dist, now_t);
		theta.next 	= theta.start + sin_accel_pos(duration, theta.dist, now_t);
		phi.next 	= phi.start + sin_accel_pos(duration, phi.dist, now_t);
		break;
	}
}
