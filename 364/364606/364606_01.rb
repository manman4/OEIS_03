n = 10000
s = 1
cnt = 0
ary = []
(0..n).each{|i|
  str = s.to_s
  if str.split('').map(&:to_i).inject(:+) % str.size == 0
    cnt += 1
    print cnt
    print ' '
    puts i
    ary << i
  end
  s *= 2
}
p ary
