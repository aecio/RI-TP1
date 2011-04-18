#include <iostream>
//#include "OccurrenceFileTests.cpp"
#include "IndexWriterTests.cpp"
#include "VocabularyTests.cpp"
#include "SequenceFileTests.cpp"

using namespace std;

int main (int argc, char const* argv[]) {
	cout << "=============================" << endl;
	cout << "Starting Test Suite" << endl;
	cout << "=============================" << endl;
	
//	occurence_file_test_cases();
	indexwriter_test_cases();
	vocabulary_test_cases();
	sequence_file_test_cases();
	
	cout << "=============================" << endl;
	cout << "Tests finished SUCCESSFULLY." << endl;
	cout << "=============================" << endl;
	
	return EXIT_SUCCESS;
}

