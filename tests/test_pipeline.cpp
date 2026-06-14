#include <gtest/gtest.h>
#include "../simulator/TelemetrySimulator.h"
#include "../parser/RocketTelemetryParser.h"
#include <vector>

TEST(PipelineIntegrationTest, SimulatorToParser) {
    TelemetrySimulator simulator(42, 50.0);
    RocketTelemetryParser parser;

    // Generate a few frames
    std::vector<DataFrame> parsed_frames;
    for (int i = 0; i < 5; ++i) {
        std::vector<uint8_t> frame_data = simulator.generateFrame();

        // Feed directly to parser
        std::vector<DataFrame> new_frames = parser.parse(frame_data);
        parsed_frames.insert(parsed_frames.end(), new_frames.begin(), new_frames.end());
    }

    // Verify
    EXPECT_EQ(parsed_frames.size(), 5);

    // Verify monotonicity of timestamp and sequence numbers
    for (size_t i = 1; i < parsed_frames.size(); ++i) {
        EXPECT_GT(parsed_frames[i].timestamp_ext, parsed_frames[i-1].timestamp_ext);
        EXPECT_EQ(parsed_frames[i].rx_seq_ext, parsed_frames[i-1].rx_seq_ext + 1);
        EXPECT_EQ(parsed_frames[i].tx_seq_ext, parsed_frames[i-1].tx_seq_ext + 1);
    }
}
