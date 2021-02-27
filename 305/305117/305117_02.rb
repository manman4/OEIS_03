def A(n)
  n1 = Math.sqrt(n).to_i
  a = Array.new(n + 1, 0)
  (-n1).upto(n1){|i|
    (-n1).upto(n1){|j|
      if i.gcd(j) == 1
        k = i * i + j * j
        if k <= n
          a[k] += 1
        end
      end
    }
  }
  b = []
  s = 0
  (0..n).each{|i|
    s += a[i]
    b << s / 4
  }
  b
end

m = 20000
ary = A(m)
(0..10000).each{|i|
  print i
  print ' '
  puts ary[i]
}
