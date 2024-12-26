require 'prime'

p_ary = Prime.each(10 ** 9).to_a
ary = [ ]
cnt = 0
d = 10
p_ary.each{|i|
  j = i ** 2
  if j < d
    cnt += 1
  elsif j < d * 10
    ary << cnt
    cnt = 1
    d *= 10
  else
    print 'error'
  end
}
p ary
  