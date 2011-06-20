#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WGroupBox>
#include <Wt/WTemplate>
#include <Wt/WAnchor>
#include <Wt/WGroupBox>
#include <Wt/WRadioButton>
#include <Wt/WButtonGroup>
#include <Wt/WImage>
#include <search/IndexSearcher.h>
#include <search/VSMIndexSearcher.h>
#include <search/BM25IndexSearcher.h>
#include <search/CombinedIndexSearcher.h>
#include <search/MultiFieldIndexSearcher.h>
#include <search/PageRankSearcher.h>

using namespace Wt;

class WebSearcher : public WApplication {
public:

	WebSearcher(const WEnvironment& env);
	
private:
	enum IRModel { BM25 = 1, Vector = 2, Combined = 3, MultiField = 4, PageRank = 5};
	
	static IndexReader* reader;
	IndexSearcher* searcher;
	WButtonGroup* bgModel;
	WLineEdit *edtQuery;
	WGroupBox *gbxResults;		

	void search();
	void btnVectorHandler();
	void btnBM25Handler();
	void btnMixedModelHandler();
	void btnMultiFieldModelHandler();
	void btnPageRankModelHandler();
};

IndexReader* WebSearcher::reader = new IndexReader("indice");

WebSearcher::WebSearcher(const WEnvironment& env) : WApplication(env) {
	setTitle("Web Searcher");
	
	root()->setMargin(200, Left);
	root()->setMargin(200, Right);

//	root()->addWidget(new WText("What are you looking for?<br/>"));
	WImage *img = new WImage("logo-dcc640x428.gif", root());
	img->setAttributeValue("style", "display:block; height: 100px;");
	img->setAlternateText("Web Searcher");

	edtQuery = new WLineEdit(root());
	edtQuery->setFocus();
	edtQuery->setTextSize(50);
	edtQuery->setMargin(15, Top);
	edtQuery->setMargin(15, Bottom);
	
	WPushButton *b = new WPushButton("Search", root());
	b->setMargin(10, Left);
	root()->addWidget(new WBreak());

	gbxResults = NULL;

	b->clicked().connect(this, &WebSearcher::search);
	edtQuery->enterPressed().connect(boost::bind(&WebSearcher::search, this));
	
	WGroupBox *container = new WGroupBox("Information Retrieval Model", root());

	// use a button group to logically group the options
	WButtonGroup *bgModel = new WButtonGroup(this);

	WRadioButton *button;
	button = new WRadioButton("BM25", container);
	button->clicked().connect(this, &WebSearcher::btnBM25Handler);
	bgModel->addButton(button, BM25);

	button = new WRadioButton("Vector Space Model", container);
	button->clicked().connect(this, &WebSearcher::btnVectorHandler);
	bgModel->addButton(button, Vector);
	
	button = new WRadioButton("VSM and BM25 combined", container);
	button->clicked().connect(this, &WebSearcher::btnMixedModelHandler);
	bgModel->addButton(button, Combined);

	button = new WRadioButton("Multi Field", container);
	button->clicked().connect(this, &WebSearcher::btnMultiFieldModelHandler);
	bgModel->addButton(button, MultiField);

	button = new WRadioButton("PageRank", container);
	button->clicked().connect(this, &WebSearcher::btnPageRankModelHandler);
	bgModel->addButton(button, PageRank);

	bgModel->setCheckedButton(bgModel->button(BM25));
	
//	reader = new IndexReader("indice");
	searcher = new BM25IndexSearcher(reader);
}

void WebSearcher::btnVectorHandler(){
	cout << "Changing model to: Vector Space Model." << endl;
	delete searcher;
	searcher = new VSMIndexSearcher(reader);
}

void WebSearcher::btnMultiFieldModelHandler(){
	cout << "Changing model to: Multi Field BM25 Model." << endl;
	delete searcher;
	searcher = new MultiFieldIndexSearcher(reader);
}

void WebSearcher::btnBM25Handler(){
	cout << "Changing model to: BM25." << endl;
	delete searcher;
	searcher = new BM25IndexSearcher(reader);
}

void WebSearcher::btnMixedModelHandler(){
	cout << "Changing model to: VSM & BM25 Combined." << endl;
	delete searcher;
	searcher = new CombinedIndexSearcher(reader);
}

void WebSearcher::btnPageRankModelHandler(){
	cout << "Changing model to: PageRank & BM25" << endl;
	delete searcher;
	searcher = new PageRankSearcher(reader);
}

void WebSearcher::search() {
	if(gbxResults != NULL) {
		root()->removeWidget(gbxResults);
	}
	gbxResults = new WGroupBox("Search results for " + edtQuery->text() + ":");
	root()->addWidget(gbxResults);

	
	vector<Hit> hits = searcher->search(edtQuery->text().toUTF8 () , 20);
		
	if(hits.size() == 0){
		WText* score = new WText(gbxResults);
		score->setText("0 documents found.");
	} else {
		vector<Hit>::iterator it = hits.begin();
		for(; it != hits.end(); it++){
			WAnchor* title = new WAnchor(gbxResults);
			title->setText(it->doc.getTitle());
			title->setRef(it->doc.getUrl());
			new WBreak(gbxResults);

			string desc = it->doc.getDescription();
			std::remove(desc.begin(), desc.end(), ' ');
			if(desc != ""){
				WText* description = new WText(gbxResults);
				description->setText(it->doc.getDescription());
				new WBreak(gbxResults);
			}

			WAnchor* link = new WAnchor(gbxResults);
			link->setText(it->doc.getUrl());
			link->setRef(it->doc.getUrl());
			link->setAttributeValue	("style", "color:green;text-decoration:none;");
			new WBreak(gbxResults);

			WText* score = new WText(gbxResults);
			WString scoreStr("Score: {1}");
			scoreStr.arg(it->score);
			score->setText(scoreStr);
			
			new WBreak(gbxResults);
			new WBreak(gbxResults);		
		}
	}
	
	edtQuery->setFocus();
}

WApplication *createApplication(const WEnvironment& env) {
  return new WebSearcher(env);
}

int main(int argc, char **argv) {
	cout << "Iniciando WebSearcher..." << endl;
	return WRun(argc, argv, &createApplication);
}
