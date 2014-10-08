/**
 * @file longboard_controller_internal.h
 * @brief 
 * @author Travis Lane
 * @version 
 * @date 2014-10-08
 */


#ifndef LONGBOARD_CONTROLLER_INTERNAL_H
#define LONGBOARD_CONTROLLER_INTERNAL_H

#define LC_WHEELS_PER_TRUCK 2

#include <stdint.h>
#include <longboard_controller.h>



/**
 * @brief A struct representing a wheel.
 */
struct lc_wheel_spec_t {
	/**
	 * @brief Width of the wheel.
	 */
	uint8_t lcw_width;

	/**
	 * @brief Diameter of the wheel.
	 */
	uint8_t lcw_diameter;
};

/**
 * @brief A struct representing a truck with two wheels.
 */
struct lc_truck_spec_t {
	/**
	 * @brief The specifications of the attached wheels.
	 * Please use two of the same size wheels...
	 */
	struct lc_wheel_spec_t lcts_wheel_spec;

	/**
	 * @brief The stated width of the truck.
	 */
	uint16_t lcts_width;

	/**
	 * @brief The actual center of wheel to center of wheel width. 
	 * If set by hand this will be exact, otherwise it is an estimate.
	 */
	uint16_t lcts_center_width;
};

/**
 * @brief The master throttle
 */
struct lc_throttle_t {

	/**
	 * @brief The specifications of the attached truck and wheels.
	 */
	struct lc_truck_spec_t lct_truck_spec;
};

#endif /* LONGBOARD_CONTROLLER_INTERNAL_H */
