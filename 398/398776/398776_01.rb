# A398776 (Erdős Problem 427)
# T(n,k) = least m >= 1 such that k divides prime(n+1) + prime(n+2) + ... + prime(n+m),
#          n >= 0, k >= 1, square array read by antidiagonals downwards:
#          T(0,1); T(0,2), T(1,1); T(0,3), T(1,2), T(2,1); ...
# 反対角線 t = n + k - 1 を t = 0..139 まで（140*141/2 = 9870 項）計算し、
# 通し番号 0 から "i T(n,k)" 形式で b398776_01.txt に保存する。
#
# T(n,k) は必ず存在する（Shiu の定理による。Erdős Problem 427 は肯定的に解決済み）ので、
# m = 1, 2, ... と順に調べるループは必ず止まる。上界は事前に分からないため、
# 素数表は足りなくなるたびに倍の範囲でふるい直す（固定の上限で打ち切らない）。
#
# 実行: ruby 398776_01.rb [T_MAX]   （T_MAX は最後の反対角線の番号、既定値 139）

T_MAX = if ARGV.empty?
          139
        elsif ARGV[0] =~ /\A\d+\z/ && ARGV[0].to_i <= 10_000
          ARGV[0].to_i
        else
          abort "usage: ruby 398776_01.rb [T_MAX]   (0 <= T_MAX <= 10000)"
        end

FNAME = "b398776_01.txt"

# primes[i] = prime(i+1)（0 始まり）
$primes = []
$limit = 1000

def sieve!(limit)
  is = Array.new(limit + 1, true)
  is[0] = is[1] = false
  i = 2
  while i * i <= limit
    if is[i]
      (i * i).step(limit, i) { |j| is[j] = false }
    end
    i += 1
  end
  $primes = (2..limit).select { |x| is[x] }
end

sieve!($limit)

# prime(idx+1) を返す。足りなければ範囲を倍にしてふるい直す
def prime_at(idx)
  while idx >= $primes.size
    $limit *= 2
    sieve!($limit)
  end
  $primes[idx]
end

# T(n,k): 和を k で割った余りだけを追う（m = 1 から）
def t_value(n, k)
  m = 0
  s = 0
  loop do
    s = (s + prime_at(n + m)) % k
    m += 1
    return m if s == 0
  end
end

# 結果ファイルがシンボリックリンクや通常ファイル以外なら置き換えない
if File.symlink?(FNAME) || (File.exist?(FNAME) && !File.file?(FNAME))
  abort "#{FNAME} is not a regular file; refusing to overwrite"
end

lines = []
idx = 0
(0..T_MAX).each do |t|
  (0..t).each do |n|          # 反対角線を下向きに: n = 0, 1, ..., t
    k = t + 1 - n
    lines << "#{idx} #{t_value(n, k)}\n"
    idx += 1
  end
end

# 一時ファイルに全体を書いてから置き換える（途中で止まっても元のファイルは残る）
tmp = "#{FNAME}.tmp.#{Process.pid}"
begin
  File.open(tmp, File::WRONLY | File::CREAT | File::EXCL, 0o644) do |f|
    f.write(lines.join)
    f.flush
    f.fsync
  end
  File.rename(tmp, FNAME)
rescue StandardError => e
  File.delete(tmp) if File.exist?(tmp)
  abort "write failed: #{e.message}"
end

puts "wrote #{idx} terms (antidiagonals 0..#{T_MAX}) to #{FNAME}"
