#pragma once

template<class T>
class RingBuffer {
private:
	T* list;
	int size;
	int startIdx = 0;
	int nextWrite = 0;
	int numElements = 0;

public:
	RingBuffer(int size) {
		this->size = size;
		list = new T[size];
	}

	~RingBuffer() {
		printf("free called\n");
		delete list;
	}

	void push(T item) {
		list[nextWrite] = item;
		nextWrite = (nextWrite + 1) % size;
		if (numElements < size)
			numElements++;
		else
			startIdx++; // Vorheriger Start wurde überschrieben
	}

	T get(int i) {
		if (numElements == 0)
			return T();
		i = (i + startIdx) % size;
		return list[i];
	}

	int getNumElements() {
		return numElements;
	}
};