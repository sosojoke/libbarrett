/*
 * system.cpp
 *
 *  Created on: Sep 19, 2009
 *      Author: dc
 */

#include <vector>
#include <gtest/gtest.h>
#include "../../systems.h"
#include "./exposed_io_system.h"


namespace {


using Systems::checkConnected;
using Systems::checkNotConnected;
using Systems::checkDisconnected;


// TODO(dc): make this a type-parameterized test case so users can test their
// own code
class SystemTest : public ::testing::Test {
protected:
	Systems::ExposedIO<double> eios;
};


TEST_F(SystemTest, GeneralIO) {
	Systems::connect(eios.output, eios.input);
	EXPECT_FALSE(eios.inputValueDefined())
		<< "input value not initially undefined when no initial value given";

	// set outputValue, then make sure value is defined and correct
	checkConnected(&eios, eios, 12.2);

	eios.setOutputValueUndefined();
	EXPECT_FALSE(eios.inputValueDefined())
		<< "input value defined after call to outputValue.setValueUndefined()";

	// set outputValue, then make sure value is defined and correct
	checkConnected(&eios, eios, 145.0);
}

TEST_F(SystemTest, InputGetValueThrowsWhenNotConnected) {
	EXPECT_THROW(eios.getInputValue(), std::logic_error)
		<< "input.getValue() didn't throw when not connected";
}

TEST_F(SystemTest, InputGetValueThrowsWhenUndefined) {
	Systems::connect(eios.output, eios.input);

	EXPECT_THROW(eios.getInputValue(),
			Systems::System::Input<double>::ValueUndefinedError)
		<< "input.getValue() didn't throw when value undefined";
}

TEST_F(SystemTest, OutputInitialValueConstructor) {
	Systems::System::Output<double>::Value* outputValue;
	Systems::System::Output<double> output(38.12, &outputValue);
	Systems::connect(output, eios.input);

	EXPECT_TRUE(eios.inputValueDefined())
		<< "input undefined when connected to output with initial value";
	EXPECT_EQ(38.12, eios.getInputValue())
		<< "input has wrong value when connected to output with initial value";
}

TEST_F(SystemTest, OutputNotifyInputs) {
	const size_t numInputs = 50;

	std::vector<Systems::ExposedIO<double>* > systems(numInputs);
	for (size_t i = 0; i < numInputs; ++i) {
		systems[i] = new Systems::ExposedIO<double>;
		Systems::connect(eios.output, systems[i]->input);
		systems[i]->operateCalled = false;
	}

	eios.setOutputValue(-87.1);

	for (size_t i = 0; i < numInputs; ++i) {
		EXPECT_TRUE(systems[i]->operateCalled)
			<< "operate() didn't get called on system " << i;

		delete systems[i];
	}
}


}
