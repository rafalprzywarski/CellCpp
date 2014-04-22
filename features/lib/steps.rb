require './buildpath'
require 'open3'
require 'rspec'
require 'fileutils'

PLAYGROUND_DIRECTORY="test_playground"
CELL_BUILD_CMD = File.expand_path("#{CELL_INSTALLATION_DIRECTORY}/usr/local/bin/cell")

class Context
  attr_accessor :cell_stdout, :cell_stderr, :cell_status, :file_timestamps
end

class Playground
  def initialize directory
    @directory = directory
    Dir.mkdir @directory
  end
  def destroy
    FileUtils.rm_rf @directory
  end
  def capture3 cmd
    Dir.chdir(@directory) do
        Open3.capture3 cmd
    end
  end
  def capture2 cmd
    Dir.chdir(@directory) do
        Open3.capture2 cmd
    end
  end
  def write_file filename, content
    File.open(@directory+ "/" + filename, "w") { |f| f.write content }
  end
  def collect_timestamps
    Dir.chdir(@directory) do
        Hash[Dir["*"].collect { |f| [ f, File.mtime(f).strftime("%Y-%m-%d %H:%M:%S.%N %z") ] }]
    end
  end
  def file_exist? filename
    File.exist?(@directory + "/" + filename)
  end
end

Before do
  $playground = Playground.new(PLAYGROUND_DIRECTORY)
  $context = Context.new
end

After do
  $playground.destroy
end

When /^I run cell$/ do
  $context.cell_stdout, $context.cell_stderr, $context.cell_status = $playground.capture3(CELL_BUILD_CMD)
end

Then /^it should fail with "(.*?)"$/ do |message|
  $context.cell_status.success?.should be_false
  $context.cell_stderr.should include message
end

Given /^file "(.*?)" with:$/ do |filename, content|
  $playground.write_file filename, content
end

Then /^it should not fail$/ do
  $context.cell_status.success?.should be_true
  $context.cell_stderr.should be_empty, $context.cell_stderr
end

Then /^compiled program "(.*?)" should print:$/ do |program, content|
  program_stdout, program_status = $playground.capture2("./" + program)
  program_stdout.should eq(content)
end

Then /^object files "(.*?)" should exist$/ do |objectFilenames|
  objectFilenames.split.each do |objectFilename|
      $playground.file_exist?(objectFilename).should be_true, "#{objectFilename} does not exist"
  end
end

Given /^default project$/ do
  $playground.write_file "build.cell", "project: default"
  $playground.write_file "main.cpp", "int main() { }\n"
end

When /^I keep timestamps of all files$/ do
  $context.file_timestamps = $playground.collect_timestamps
end

Then /^timestamps should not change$/ do
  current_file_timestamps = $playground.collect_timestamps
  current_file_timestamps.should eq($context.file_timestamps)
end
