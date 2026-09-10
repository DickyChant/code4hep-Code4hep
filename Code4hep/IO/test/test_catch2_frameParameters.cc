#include "catch2/catch_all.hpp"

#include "Code4hep/IOUtilities/CollectionNameConversion.h"
#include "Code4hep/IOUtilities/FrameParameterConversion.h"
#include "podio/Frame.h"

#include <string>
#include <vector>

TEST_CASE("Podio collection names are reversible framework labels", "[CollectionNames]") {
  REQUIRE(c4h::collectionNameToProductLabel("EventHeader") == "EventHeader");
  for (auto const& name : {"sDST_EVT_BField", "c4hPodioEncoded", "a_b_c"}) {
    const auto label = c4h::collectionNameToProductLabel(name);
    REQUIRE(label.find('_') == std::string::npos);
    REQUIRE(c4h::productLabelToCollectionName(label) == name);
  }
}

TEST_CASE("Frame parameters survive framework materialization", "[FrameParameters]") {
  podio::Frame input;
  input.putParameter("int_key", std::vector<int>{-7, 42});
  input.putParameter("float_key", std::vector<float>{1.25f, -2.5f});
  input.putParameter("double_key", std::vector<double>{3.125, -9.5});
  input.putParameter("string_key", std::vector<std::string>{"94C2", "", "a_b"});

  c4h::materializeFrameParameters(input);
  REQUIRE(input.getAvailableCollections().size() == 4);

  podio::Frame output;
  for (auto const& name : input.getAvailableCollections()) {
    REQUIRE(c4h::restoreFrameParameter(output, name, *input.get(name)));
  }

  REQUIRE(output.getParameter<std::vector<int>>("int_key") == std::vector<int>{-7, 42});
  REQUIRE(output.getParameter<std::vector<float>>("float_key") == std::vector<float>{1.25f, -2.5f});
  REQUIRE(output.getParameter<std::vector<double>>("double_key") == std::vector<double>{3.125, -9.5});
  REQUIRE(output.getParameter<std::vector<std::string>>("string_key") ==
          std::vector<std::string>{"94C2", "", "a_b"});
}
