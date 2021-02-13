def A(n)
  a = [1]
  ary = [1]
  (n - 1).times{
    b = []
    a.each{|i|
      if i == 1
        b << 1
        b << 2
      elsif i == 2
        b << 1
        b << 3
      elsif i == 3
        b << 1
        b << 4
      else
        b << 1
      end
    }
    a = b
    ary << a.join.to_i
  }
  ary
end

n = 20
ary = A(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
