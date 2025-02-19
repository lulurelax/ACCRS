#include "utils.h"





bool sortByX(const Rect& lhs, const Rect& rhs) {
	if (lhs.x<rhs.x)
		return true;
	else return false;
}

bool sortClusterByY(const vector<Rect>& lhs, const vector<Rect>& rhs) {
	if (lhs[0].y<rhs[0].y)
		return true;
	else return false;
}
int isClusterExist(Rect rect, vector<vector<Rect>> clusters, vector<float> rectsx, vector<float> rectsy, vector<float> rectsheight, int xthres, int ythres, int heightthres) {
	int n = rectsx.size();
	float x = rect.x, y = rect.y + rect.height / 2.0;
	for (int i = 0; i<n; i++) {
		if (abs(x - rectsx[i])<xthres)
			if (abs(y - rectsy[i])<ythres)
				if (abs(rect.height - rectsheight[i])<heightthres)
					return i;
	}
	return -1;
}

void clusteringRects(vector<Rect> rects, vector<vector<Rect>>& clusters, int xthres, int ythres, int heightthres) {
	//vector<vector<Rect>> clusters;
	vector<float> rectsx, rectsy, rectsheight;
	sort(rects.begin(), rects.end(), sortByX);
	for (auto rect : rects) {

		int index = isClusterExist(rect, clusters, rectsx, rectsy, rectsheight, xthres, ythres, heightthres);
		if (index == -1) {
			vector<Rect> newcluster;
			newcluster.push_back(rect);
			clusters.push_back(newcluster);
			float newx = rect.x + rect.width, newy = rect.y + rect.height / 2.0;
			rectsx.push_back(newx);
			rectsy.push_back(newy);
			rectsheight.push_back(rect.height);
		}
		else {

			rectsx[index] = rect.x + rect.width;
			rectsy[index] = (clusters[index].size()*rectsy[index] + rect.y + rect.height / 2.0) / (clusters[index].size() + 1);
			rectsheight[index] = (clusters[index].size()*rectsheight[index] + rect.height) / (clusters[index].size() + 1);
			clusters[index].push_back(rect);
		}
	}

	sort(clusters.begin(), clusters.end(), sortClusterByY);
	//sortCluster(clusters);
	//sort(clusters.begin(),clusters.end(),sortByClusterSize);
	//return clusters;
}




void removeOverlap(vector<Rect>& rects,vector<Rect>& slim_rects) {
	sort(rects.begin(), rects.end(), sortByX);
	int n = rects.size();
	int* sign = new int[n] {1};
	for (int i = 0; i < n; i++)
		sign[i] = 1;
	for (int i = 0; i < n; i++) {
		Rect rect1 = rects[i];
		for (int j = i+1; j < n; j++) {
			Rect rect2 = rects[j];
			float x0 = std::max(rect1.x, rect2.x);
			float x1 = std::min(rect1.x + rect1.width, rect2.x + rect2.width);
			float y0 = std::max(rect1.y, rect2.y);
			float y1 = std::min(rect1.y + rect1.height, rect2.y + rect2.height);
			if (x1 <= x0 || y1 <= y0)
				break;
			float overlapArea = (x1 - x0)*(y1 - y0);
			float area1 = rect1.width*rect1.height;
			float area2 = rect2.width*rect2.height;
			if (overlapArea / area1 > 0.8 && overlapArea / area2 > 0.8) {
				sign[j] = 0;
			}
		}
	}
	for (int i = 0; i < n; i++) {
		if (sign[i] == 1)
			slim_rects.push_back(rects[i]);
	}
}

bool isOverlap(const Rect &rc1, const Rect &rc2)
{
	if (rc1.x + rc1.width  > rc2.x &&
		rc2.x + rc2.width  > rc1.x &&
		rc1.y + rc1.height > rc2.y &&
		rc2.y + rc2.height > rc1.y
		)
		return true;
	else
		return false;
}


void scaleSingleRect(Rect& rect, float scalex, float scaley, int maxx, int maxy) {
	int x = rect.x;
	int y = rect.y;
	int width = rect.width;
	int height = rect.height;
	x = x - ceil((float)width*scalex / 2.0);
	y = y - ceil((float)height*scaley / 2.0);
	width += width*scalex;
	height += height*scaley;
	rect.x = x < 0 ? 0 : x;
	rect.y = y < 0 ? 0 : y;
	rect.width = width;
	rect.height = height;
	if (x + width + 2>maxx)
		rect.width = maxx - x - 2;
	if (y + height + 2>maxy)
		rect.height = maxy - y - 2;
}


void mergeVerifyNum(vector<Rect>& rects) {
	int n = rects.size();
	if (n > 2) {
		Rect rect1 = rects[n - 1];
		Rect rect2 = rects[n - 2];
		rects.pop_back();
		rects.pop_back();
		Rect rect = rect1 | rect2;
		rects.push_back(rect);
	}
	

}

int clusterInCluster(vector<Rect> cluster) {
	vector<vector<Rect>> clusters;
	for(auto rect: cluster){
		int flag = 0;
		int n = clusters.size();
		for (int i = 0; i < n;i++) {
			int isOverlapped = 0;
			for (auto rect2 : clusters[i]) {
				if (isOverlap(rect, rect2)) {
					isOverlapped = 1;
					break;
				}
			}
			if (isOverlapped == 1) {
				clusters[i].push_back(rect);
				flag = 1;
				break;
			}
		}
		if (flag == 0) {
			vector<Rect> newcluster;
			newcluster.push_back(rect);
			clusters.push_back(newcluster);
		}

	}
	return clusters.size();
}

void findClusterAttr(vector<vector<Rect>>& clusters, vector<int> &attr) {
	for (auto cluster : clusters) {
		attr.push_back(clusterInCluster(cluster));
	}
}