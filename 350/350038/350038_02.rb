def A(n)
  ary = []
  (1..n).each{|i|
    (i + 1..n).each{|j|
      if i.gcd(j) == 1 && (i - j) % 3 > 0
        x, y, z = j * j, i * j, i * i
        ary << 2 * x + 5 * y + 2 * z
        ary << 3 * x + 3 * y
      end
    }
  }
  ary
end

n = 10000
# include?()でlistに入れていくより、あとで数えた方が速い

ary = A(10000).uniq.sort[0..n - 1]
(1..n).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
