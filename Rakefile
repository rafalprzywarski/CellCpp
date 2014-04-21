require 'rake/clean'
require './buildpath'

CLOBBER.include(CELL_BUILD_DIRECTORY)
CLOBBER.include(CELL_INSTALLATION_DIRECTORY)

task :default => :acceptance_tests

task :cell do
  build_cmake_targets CELL_BUILD_DIRECTORY, [ :celltest, :cell ]
  sh "#{CELL_BUILD_DIRECTORY}/celltest"
end

task :install_cell => :cell do
  sh "(cd #{CELL_BUILD_DIRECTORY}; make install DESTDIR=#{absolute_path(CELL_INSTALLATION_DIRECTORY)})"
end

task :acceptance_tests => :install_cell do
  sh 'cucumber -p acceptance_tests'
end

task :wip => :install_cell do
  sh 'cucumber -p wip'
end


PROJECT_DIRECTORY=File.dirname(__FILE__)

def absolute_path dir
  PROJECT_DIRECTORY + "/" + dir
end

def build_cmake_target path, target
  sh "cmake -E make_directory #{path}"
  sh "cmake -E chdir #{path} cmake .."
  sh "cmake --build #{path} --target #{target.to_s}"
end

def build_cmake_targets path, targets
  targets.each { |target| build_cmake_target path, target }
end

