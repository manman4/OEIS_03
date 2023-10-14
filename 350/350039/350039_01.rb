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

# include?()でlistに入れていくより、あとで数えた方が速い
p A(100).group_by(&:to_i).select{|k, v| v.size > 1}.keys.sort[0..50]
