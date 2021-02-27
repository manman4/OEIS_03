def A(n)
  return 1 if n == 0
  ary = [[0]]
  (1..n).each{|i|
    ary1 = Marshal.load(Marshal.dump(ary))
    ary0 = ary1.map{|i| i.clone}
    (1..n / i).each{|j|
      s = i * j
      ary1.each{|a|
        b = a.clone
        sum = b.inject(:+)
        if (sum + s) <= n && b[-1] < s
          b << s
          ary0 << b
        end
      }
      ary = ary0
    }
  }
  ary.select{|i| i.inject(:+) == n}.size
end

n = 10
(0..n).each{|i|
  print i
  print ' '
  puts A(i)
}
