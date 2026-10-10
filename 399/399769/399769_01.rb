# Erdős Problem #696: H(n)
# H(n) = n の約数 d_1 < ... < d_u で
#        d_{i+1} ≡ 1 (mod d_i) を満たす列の最大長 u
# n = 1..10000 について計算し b399769_01.txt に "n H(n)" 形式で保存
# （定義どおり d = 1 も約数に含める。任意の整数は ≡ 1 (mod 1)）

N = 10000

def divisors(n)
  small = []
  large = []
  i = 1
  while i * i <= n
    if n % i == 0
      small << i
      large << n / i if i != n / i
    end
    i += 1
  end
  small + large.reverse # 昇順
end

def big_h(n)
  ds = divisors(n)
  len = Array.new(ds.size, 1)
  ds.each_with_index do |d, j|
    (0...j).each do |i|
      len[j] = len[i] + 1 if (d - 1) % ds[i] == 0 && len[i] + 1 > len[j]
    end
  end
  len.max
end

File.open('b399769_01.txt', 'w') do |f|
  (1..N).each { |n| f.puts "#{n} #{big_h(n)}" }
end
