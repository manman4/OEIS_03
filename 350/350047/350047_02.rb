def A(n)
  ary = []
  (1..n).each{|i|
    (i + 1..n).each{|j|
      if i.gcd(j) == 1 && (i - j) % 3 > 0
        ary << 2 * j * j + 3 * i * j + i * i
      end
    }
  }
  ary
end

n = 10000
# include?()でlistに入れていくより、あとで数えた方が速い

# a(10000)=4857360<2*2000^2なので2000でOK
ary = A(10000).group_by(&:to_i).select{|k, v| v.size > 1}.keys.sort[0..n - 1]
(1..n).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
