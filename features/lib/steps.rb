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
    File.open(file_path(filename), "w") { |f| f.write content }
  end
  def write_executable_file filename, content
    write_file filename, content
    File.chmod 0777, file_path(filename)
  end
  def read_file filename
    File.read(file_path(filename))
  end
  def collect_timestamps
    Dir.chdir(@directory) do
        Hash[Dir["*"].collect { |f| [ f, File.mtime(f).strftime("%Y-%m-%d %H:%M:%S.%N %z") ] }]
    end
  end
  def file_exist? filename
    File.exist? file_path(filename)
  end
  def touch filename
    FileUtils.touch file_path(filename)
  end
private
  def file_path filename
    "#{@directory}/#{filename}"
  end
end

Before do
  $playground = Playground.new(PLAYGROUND_DIRECTORY)
  $context = Context.new
end

After do
  $playground.destroy
end

def sleepForTimestampResolution
    sleep 1
end

When /^I run cell$/ do
  $context.cell_stdout, $context.cell_stderr, $context.cell_status = $playground.capture3(CELL_BUILD_CMD)
  sleepForTimestampResolution
end

Then /^it should fail with "(.*?)"$/ do |message|
  $context.cell_status.success?.should be_false
  $context.cell_stderr.should include message
end

Given /^file "(.*?)" with:$/ do |filename, content|
  $playground.write_file filename, content
end

Then /^it should not fail$/ do
  $context.cell_status.success?.should be_true, "it failed with status: #{$context.cell_status}, stdout:\n#{$context.cell_stdout}\nstderr:\n#{$context.cell_stderr}"
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

Given /^project:$/ do |content|
  $playground.write_file "build.cell", content
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

Given(/^project file with:$/) do |content|
  $playground.write_file "build.cell", content
end

Given /^project "(.*?)" with main source "(.*?)" and sources "(.*?)"$/ do |project_name, main_source_filename, source_filenames|
  $playground.write_file "build.cell", "project: #{project_name}"
  step "main source \"#{main_source_filename}\""
  step "sources \"#{source_filenames}\""
end

When /^I touch "(.*?)"$/ do |filename|
  $playground.touch filename
end

Then /^only files "(.*?)" should change$/ do |filenames|
  current_file_timestamps = $playground.collect_timestamps
  $context.file_timestamps.keys.each do |f|
    current_file_timestamps[f].should_not eq($context.file_timestamps[f]), "#{f} did not change" if filenames.include? f
    current_file_timestamps[f].should eq($context.file_timestamps[f]), "#{f} changed" unless filenames.include? f
  end
end

Given /^main source "(.*?)"$/ do |filename|
  $playground.write_file filename, "int main() { }\n"
end

Given(/^sources "(.*?)"$/) do |filenames|
  filenames.split.each { |f| $playground.write_file f, "static void f() { }" }
end

Then /^file "(.*?)" should exist$/ do |filename|
  $playground.file_exist?(filename).should be_true, "#{filename} does not exist"
end

Then /^file "(.*?)" should not exist$/ do |filename|
  $playground.file_exist?(filename).should be_false, "#{filename} exists"
end

Given(/^a fake compiler "(.*?)"$/) do |compiler_name|
  $playground.write_executable_file compiler_name, "echo $@ >> #{compiler_name}.log\n"
end

Then(/^fakecc should run with "(.*?)"$/) do |args|
  $playground.read_file("fakecc.log").split("\n").should include(args)
end
