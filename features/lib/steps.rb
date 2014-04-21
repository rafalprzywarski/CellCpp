require './buildpath'
require 'open3'
require 'rspec'

When /^I run cell$/ do
  $CELL_STDOUT, $CELL_STDERR, $CELL_STATUS = Open3.capture3("#{CELL_INSTALLATION_DIRECTORY}/usr/local/bin/cell")
end

Then /^it should fail with "(.*?)"$/ do |message|
  $CELL_STATUS.success?.should be_false
  $CELL_STDERR.should include message
end
