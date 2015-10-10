require 'spec_helper'

#describe "cli" do
#  context "token mode" do
#    it "should accept input from stdin" do
#      expect(cli(['--tokens'])).to eq("")
#    end
#
#    it "should translate the input file" do
#      expect(cli(['--tokens', 'spec/src/sample.scl'])).to eq("")
#      expect(File.exists? 'spec/src/sample.tok').to be(true)
#      FileUtils.rm('spec/src/sample.tok')
#    end
#  end
#
#  context "ast mode" do
#    it "should accept input from stdin" do
#      expect(cli(['--ast'])).to eq("")
#    end
#
#    it "should translate the input file" do
#      expect(cli(['--ast', 'spec/src/sample.scl'])).to eq("")
#      expect(File.exists? 'spec/src/sample.ast').to be(true)
#      FileUtils.rm('spec/src/sample.ast')
#    end
#  end
#
#  context "c source mode" do
#    it "should accept input from stdin" do
#      expect(cli(['--csource'])).not_to eq("")
#    end
#
#    it "should translate the input file" do
#      expect(cli(['--csource', 'spec/src/sample.scl'])).to eq("")
#      expect(File.exists? 'spec/src/sample.c').to be(true)
#      FileUtils.rm('spec/src/sample.c')
#    end
#  end
#
#  context "object mode" do
#    it "should error when too few input files provided" do
#      expect{cli(['--object'])}.to raise_error(/too few files/)
#    end
#
#    it "should error when too many input files provided" do
#      expect{cli(['--object', 'spec/src/sample.scl', 'spec/src/sample.scl'])}.to raise_error(/too many files/)
#    end
#
#    it "should compile the input file in verbose mode (short flag)" do
#      expect(cli(['-v', '--object', 'spec/src/sample.scl'])).to eq(
#          "cc -c -o spec/src/sample.o -I ./build/bin/../include/ spec/src/sample.c\n")
#      expect(File.exists? 'spec/src/sample.o').to be(true)
#      FileUtils.rm('spec/src/sample.o')
#    end
#
#    it "should compile the input file in verbose mode (long flag)" do
#      expect(cli(['--verbose', '--object', 'spec/src/sample.scl'])).to eq(
#          "cc -c -o spec/src/sample.o -I ./build/bin/../include/ spec/src/sample.c\n")
#      expect(File.exists? 'spec/src/sample.o').to be(true)
#      FileUtils.rm('spec/src/sample.o')
#    end
#
#    it "should compile the input file" do
#      expect(cli(['--object', 'spec/src/sample.scl'])).to eq("")
#      expect(File.exists? 'spec/src/sample.o').to be(true)
#      FileUtils.rm('spec/src/sample.o')
#    end
#  end
#end

