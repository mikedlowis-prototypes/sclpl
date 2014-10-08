
module TestUtils
def self.exec_cmd(cmd, input)
  out, err, status = Open3.capture3(*cmd, :stdin_data => input)
end
end

