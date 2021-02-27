n = 100
s = 1
ary = [1]
cnt = 0
while cnt < n
  s *= 2 * cnt + 1
  ary << s.to_s.split('').inject(0){|s, i| s + i.to_i}
  cnt += 1
end
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}
