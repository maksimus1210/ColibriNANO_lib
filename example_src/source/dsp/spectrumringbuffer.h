// This is an independent project of an individual developer. Dear PVS-Studio, please check it. 
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef SPECTRUMRINGBUFFER_H
#define SPECTRUMRINGBUFFER_H

#include <atomic>
#include <vector>

#include <QtCore>
#include <QtGlobal>

namespace SDR {

using namespace std;

/**
 * \class SpectrumRingBuffer
 * \brief Кольцевой буфер для вычисления спектра.
 *
 * \details Этот класс предназначен для линейного заполнения аудио сигналом,
 * доступно чтение сразу всего содержимого. Такой буфер позволяет наполнять себя
 * малыми частями и сохранять временную последовательность сигнала. Размер буфера
 * всегда кратен степени двойки.
 * Алгоритм работы: \n
 *  1) Устанавливается размер буфера равный размеру FFT;
 *  2) Периодически записывается входной сигнал произвольной длины;
 *  3) Как только количество записанных сэмплов становится равным или большим размера буфера разрешается чтение;
 *  4) Как только был прочитан весь буфер включается запрет на чтение;
 *  5) После прихода порции новых сэмплов снова разрешается чтение всего буфера.
 */
template <typename T>
class SpectrumRingBuffer
{
public:
    /**
     * \brief Конструктор класса по умолчанию.
     * \param t_size - размер буфера.
     */
    explicit SpectrumRingBuffer(quint32 t_size = 0);


    /**
     * \brief Установка размера буфера.
     * \param t_size - размер буфера.
     *
     * \details Размера буфера всегда кратен степени двойки, если t_size не удовлетворяет
     * этому условию, то устанавливается размер больший чем t_size, но кратный степени двойки.
     */
    void resize(quint32 t_size);

    /**
     * \brief Возвращает размер буфера.
     * \return размер буфера.
     */
    quint32 size() const noexcept;


    /**
     * \brief Очистка буфера.
     *
     * \details Содержимое буфера не обнуляется, это не требуется.
     */
    void clear() noexcept;


    /**
     * \brief Заполнение буфера заданным значением.
     */
    void fill(T value);


    /**
     * \brief Возвращает статус чтения.
     * \return статус чтения.
     */
    bool readyRead() const noexcept;

    /**
     * \brief Возвращает статус записи.
     * \return статус чтения.
     */
    bool readyWrite() const noexcept;


    /**
     * \brief Чтение содержимого всего буфера.
     * \param dst - буфер в который запишутся данные.
     * \return статус выполнения.
     */
    bool readAll(vector<T> &dst) noexcept;
    bool readAll(T *pDst, quint32 len) noexcept;

    /**
     * \brief Запись в буфер.
     * \param src - буфер с входными данными.
     * \return статус выполнения.
     *
     * \details Размер входного буфера может иметь любой размер,
     * даже если входной буфер имеет размер больше, чем сам кольцевой буфер,
     * данные будут линейно записаны в буфер.
     */
    bool write(const vector<T> &src) noexcept;

    bool write(const T *pSrc, quint32 len) noexcept;

private:
    SpectrumRingBuffer(const SpectrumRingBuffer &) = delete;
    SpectrumRingBuffer &operator=(const SpectrumRingBuffer &) = delete;

    inline int pow2Next(int d)
    {
        d--;
        d |= d >> 1;
        d |= d >> 2;
        d |= d >> 4;
        d |= d >> 8;
        d |= d >> 16;
        d++;
        return d;
    }

private:
    quint32 m_size;
    quint32 m_readPtr;
    quint32 m_writePtr;
    quint32 m_mask;

    atomic_bool      m_readyRead;
    atomic<quint32> m_bytesForRead;
    atomic<quint32> m_bytesForWrite;

    vector<T> m_buffer;
    QMutex     m_mutex;
};


template <typename T>
SpectrumRingBuffer<T>::SpectrumRingBuffer(quint32 t_size) :
  m_size(t_size),
  m_readPtr(0),
  m_writePtr(0),
  m_mask(0),
  m_readyRead(false),
  m_bytesForRead(0),
  m_bytesForWrite(0)
{
    // установка размера буфера
    m_buffer.resize(pow2Next(t_size));

    // инициализация
    m_size = m_buffer.size();
    m_mask = m_size - 1;
    m_bytesForWrite = m_size;

}

template <typename T>
void SpectrumRingBuffer<T>::resize(quint32 t_size)
{
    if (m_size == t_size)
        return;

    // запрещаем запись и чтение
    m_bytesForRead  = 0;
    m_bytesForWrite = 0;

    // установка размера буфера
    m_mutex.lock();
        m_buffer.resize(pow2Next(t_size));
    m_mutex.unlock();

    // инициализация
    m_size = m_buffer.size();
    m_mask = m_size - 1;
    m_bytesForWrite = m_size;
}

template <typename T>
quint32 SpectrumRingBuffer<T>::size() const noexcept
{
    return m_size;
}

template <typename T>
void SpectrumRingBuffer<T>::clear() noexcept
{
    // запрещаем запись и чтение
    m_bytesForRead  = 0;
    m_bytesForWrite = 0;

    // сбрасываем указатели записи и чтения
    m_readPtr  = 0;
    m_writePtr = 0;

    // сбрасываем флаг, который разрешает чтение
    m_readyRead = false;

    // разрешаем запись и чтение
    m_bytesForWrite = m_size;
}

template <typename T>
void SpectrumRingBuffer<T>::fill(T value)
{
    m_mutex.lock();
        m_buffer.fill(value);
    m_mutex.unlock();
}

template <typename T>
bool SpectrumRingBuffer<T>::readyRead() const noexcept
{
    return m_readyRead;
}

template <typename T>
bool SpectrumRingBuffer<T>::readyWrite() const noexcept
{
    return m_size != 0;
}

template <typename T>
bool SpectrumRingBuffer<T>::readAll(vector<T> & dst) noexcept
{
    // проверяем доступность чтения
    if ((static_cast<quint32>(dst.size()) != m_size) || !m_readyRead)
        return false;

    // перед началом чтения сбрасываем флаг, определяющий доступность чтения
    m_readyRead = false;

    // чтение данных
    m_mutex.lock();
        // устанавливаем указатель на начало данных
        m_readPtr   = m_writePtr;

        for (T &data : dst)
            data = m_buffer[m_readPtr++&m_mask];
    m_mutex.unlock();

    // успешное выполнение
    return true;
}

template <typename T>
bool SpectrumRingBuffer<T>::readAll(T *pDst, quint32 len) noexcept
{
    // проверяем доступность чтения
    if ((len != m_size) || !m_readyRead)
        return false;

    // перед началом чтения сбрасываем флаг, определяющий доступность чтения
    m_readyRead = false;

    // чтение данных
    m_mutex.lock();
        // устанавливаем указатель на начало данных
        m_readPtr   = m_writePtr;

        for (quint32 i = 0; i < len; ++i)
            pDst[i] = m_buffer[m_readPtr++&m_mask];
    m_mutex.unlock();

    // успешное выполнение
    return true;
}

template <typename T>
bool SpectrumRingBuffer<T>::write(const vector<T> & src) noexcept
{
    // проверяем доступность записи
    if ((src.size() == 0) && (m_size == 0u))
        return false;

    // запись данных
    m_mutex.lock();
        for (T &data : const_cast<vector<T> &>(src))
            m_buffer[m_writePtr++&m_mask] = data;
        m_writePtr &= m_mask;
    m_mutex.unlock();

    // определяем количество доступных для чтения сэмплов
    m_bytesForRead = qMin<quint32>(m_bytesForRead + src.size(), m_size);
    //устанавливаем флаг, определяющий доступность чтения
    m_readyRead    = m_bytesForRead == m_size;

    // успешное выполнение
    return true;
}

template <typename T>
bool SpectrumRingBuffer<T>::write(const T *pSrc, quint32 len) noexcept
{
    // проверяем доступность записи
    if ((len == 0u) && (m_size == 0u))
        return false;

    // запись данных
    m_mutex.lock();
        for (quint32 i = 0; i < len; ++i)
            m_buffer[m_writePtr++&m_mask] = pSrc[i];
        m_writePtr &= m_mask;
    m_mutex.unlock();

    // определяем количество доступных для чтения сэмплов
    m_bytesForRead = qMin<quint32>(m_bytesForRead + len, m_size);
    //устанавливаем флаг, определяющий доступность чтения
    m_readyRead    = m_bytesForRead == m_size;

    // успешное выполнение
    return true;
}

}

#endif // SPECTRUMRINGBUFFER_H
