ls = STDIN.read.lines[1..-2].map(&:chomp).map { |l| [l[0..-3].to_i, l[-1].to_i] }
edges, _ = ls.reduce([[], ls[0]]) do |(res, (last_time, last_state)), (time, state)|
  unless last_time < time
    [res, [last_time, last_state]]
  else
    new_res = (state == last_state ? res : res+[time])
    [new_res, [time, state]]
  end
end
edges = edges.take(21)
durations = edges.each_cons(2).map { |a, b| b - a }
average = durations.reduce(&:+) / durations.size.to_f
STDERR.puts "edges: #{edges}(#{edges.size})"
STDERR.puts "durations: #{durations}(#{durations.size})"
STDERR.puts "average of switching time: #{average}ms"

File.write('./result1.txt', average.to_s)
File.write('./result2.txt', (average/10000*1000).to_s)
