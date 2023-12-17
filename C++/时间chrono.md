# 时间库chrono 

C++11引入了boost的chrono，是一个与时间相关的库。  

> ## duration 

std::chrono::duration 表示一段时间，默认以秒表示，它是一个模板类。
```C++
template<typename _Rep, typename _Period = ratio<1>>
struct duration;
```
--- 第一个参数是数值类型，可以是int或者float；  
--- 第二个参数std::ratio也是一个模板类，可以简单理解为一个比例数值，第一个参数是分子，第二个是分母。 

```C++
typedef ratio<1,       1000000000000000000> atto;
typedef ratio<1,          1000000000000000> femto;
typedef ratio<1,             1000000000000> pico;
typedef ratio<1,                1000000000> nano;
typedef ratio<1,                   1000000> micro;
typedef ratio<1,                      1000> milli;
typedef ratio<1,                       100> centi;
typedef ratio<1,                        10> deci;
typedef ratio<                       10, 1> deca;
typedef ratio<                      100, 1> hecto;
typedef ratio<                     1000, 1> kilo;
typedef ratio<                  1000000, 1> mega;
typedef ratio<               1000000000, 1> giga;
typedef ratio<            1000000000000, 1> tera;
typedef ratio<         1000000000000000, 1> peta;
typedef ratio<      1000000000000000000, 1> exa;
```

那么如果想要定义一个step为1分钟的时间间隔类型，就是
```C++
typedef std::chrono::duration<int, std::ratio<60,1>> my_minute_type;
```
当然，我们一般不需要关注这些，绝大部分场景我们都是使用chrono中预先定义好的几种duration。
```C++
/// nanoseconds
using nanoseconds	= duration<_GLIBCXX_CHRONO_INT64_T, nano>;
/// microseconds
using microseconds	= duration<_GLIBCXX_CHRONO_INT64_T, micro>;
/// milliseconds
using milliseconds	= duration<_GLIBCXX_CHRONO_INT64_T, milli>;
/// seconds
using seconds	= duration<_GLIBCXX_CHRONO_INT64_T>;
/// minutes
using minutes	= duration<_GLIBCXX_CHRONO_INT64_T, ratio< 60>>;
/// hours
using hours		= duration<_GLIBCXX_CHRONO_INT64_T, ratio<3600>>;
/// days
using days		= duration<_GLIBCXX_CHRONO_INT64_T, ratio<86400>>;
/// weeks
using weeks		= duration<_GLIBCXX_CHRONO_INT64_T, ratio<604800>>;
/// years
using years		= duration<_GLIBCXX_CHRONO_INT64_T, ratio<31556952>>;
/// months
using months	= duration<_GLIBCXX_CHRONO_INT64_T, ratio<2629746>>;
```
比如，最常见的用法就是让当前线程sleep一段时间，
std::this_thread::sleep_for(std::chrono::seconds(2));  
有的时候会觉得这样写时间比较麻烦，借助std::literals可以进行简化。
```C++
using namespace std::literals;
std::this_thread::sleep_for(2s);
```

> # time_point

表示一个具体的时间点，也是一个模板类，实例化的时候需要指定clock类型，以duration为计量单位，默认的一般不是秒，起始时间 + duration就是当前的time_point。对于utc时间来说，起始时间就是1970的零点。
```C++
template<typename _Clock, typename _Dur = typename _Clock::duration>
struct time_point;
```
如果我们选择std::chrono::system_clock，通常就是墙上时钟，那么第二个模板单位从其定义中可以看到，是nanoserconds
```C++
struct system_clock
{
    typedef chrono::nanoseconds				duration;
```
比如我们最常见的用法是获取当前的utc时间，转成秒或者其他单位，那么就可以这么写。
```C++
#define TIME_NOW_SECONDS std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()
#define TIME_NOW_MILLISECONDS std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()

std::cout << TIME_NOW_SECONDS << std::endl;
std::cout << TIME_NOW_MILLISECONDS << std::endl;
```
system_clock会受到时间跳变的影响，那么如果是要获取一个单调的时间，我们可以用steady_clock，这个我不确定是不是从系统启动开始算0的，但是它确实是不会变化，始终单调递增的。
还有一种用法是统计代码执行的时间，此时我们可以用std::chrono::high_resolution_clock，不过这个好像就是一个system_clock的typedef。
```C++
auto start = std::chrono::high_resolution_clock::now();
std::this_thread::sleep_for(1s);
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
std::cout << "microseconds : " << duration.count();
```
需要注意的是timepoint表示的就是一个时间点，就算度量单位不同，只要是同一个时间点他们就是相同的。
```C++
std::chrono::time_point<std::chrono::system_clock> start_time;
auto time1 = start_time + std::chrono::seconds{10};
auto time2 = start_time + std::chrono::milliseconds{10000};
std::cout << "is same: " << (time1 == time2) << endl;
```
timepoint之间不能想加，可以相减得到duration。

通过to_time_t可以转换成time_t，通过from_time_t可以冲time_t转换成timepoint。
```C++
auto tpnow = std::chrono::system_clock::now();
time_t time_tnow = std::chrono::system_clock::to_time_t(tpnow);
std::cout << ctime(&time_tnow) << endl;
```
