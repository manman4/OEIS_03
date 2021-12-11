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

# a(10000)=1265616<2*1000^2なので1000でOK
ary = A(1000).group_by(&:to_i).select{|k, v| v.size > 1}.keys.sort[0..n - 1]
(1..n).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
