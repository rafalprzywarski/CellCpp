require './buildpath'
require 'open3'
require 'rspec'
require 'fileutils'

PLAYGROUND_DIRECTORY="test_playground"
CELL_BUILD_CMD = File.expand_path("#{CELL_INSTALLATION_DIRECTORY}/usr/local/bin/cell")

Before do
  Dir.mkdir PLAYGROUND_DIRECTORY
end

After do
  FileUtils.rm_rf PLAYGROUND_DIRECTORY
end

When /^I run cell$/ do
  Dir.chdir(PLAYGROUND_DIRECTORY) do
    $CELL_STDOUT, $CELL_STDERR, $CELL_STATUS = Open3.capture3(CELL_BUILD_CMD)
  end
end

Then /^it should fail with "(.*?)"$/ do |message|
  $CELL_STATUS.success?.should be_false
  $CELL_STDERR.should include message
end

Given /^file "(.*?)" with:$/ do |filename, content|
  File.open(PLAYGROUND_DIRECTORY + "/" + filename, "w") { |f| f.write content }
end

Then /^it should not fail$/ do
  $CELL_STATUS.success?.should be_true
  $CELL_STDERR.should be_empty, $CELL_STDERR
end

Then /^compiled program "(.*?)" should print:$/ do |program, content|
  Dir.chdir(PLAYGROUND_DIRECTORY) do
    program_stdout, program_status = Open3.capture2("./" + program)
    program_stdout.should eq(content)
  end
end

Then /^object files "(.*?)" should exist$/ do |objectFilenames|
  objectFilenames.split.each do |objectFilename|
      File.exist?(PLAYGROUND_DIRECTORY + "/" + objectFilename).should be_true, "#{objectFilename} does not exist"
  end
end